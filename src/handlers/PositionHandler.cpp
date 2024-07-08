// TODO: logging and error handling
#include "PositionHandler.h"

using std::vector, std::unordered_set, std::bitset, std::pair;

PositionHandler::PositionHandler(Box const bound):boundaries_(bound) {

	// putting in the inital data for tree_ and leaf_data_
	tree_.emplace_back();
	for (auto i = 0; i < 8; ++i) {
		LeafData new_leaf;
		new_leaf.leaf_box = GetOctantBox(boundaries_, Octant(i));
		new_leaf.tree_index = 0;
		leaf_data_.push_back(new_leaf);
		// update the first TreeNode's leaf_indices
		tree_[0].leaf_indices[i] = i;
	}
}

void PositionHandler::Add(EntityID const handle, Box const ent_box) {
	// get index for leaf it intersects in tree_
	// get the LeafData for that leaf
	// then try to add it to that leaf (splits if too many objects in area)
	unordered_set<u32> leaf_indices;
	FindLeafData(leaf_indices, 0, boundaries_, ent_box);
	AddToLeaf(handle, ent_box, leaf_indices);
	index_[handle] = {leaf_indices, ent_box};
}

void PositionHandler::Move(EntityID const handle, Dir const move_vec) {
	auto& ent_data = index_[handle];
	auto& ent_box = ent_data.entity_box;
	ent_box.pos += move_vec;

	u32 largest_parent_node = *ent_data.leaf_indices.begin();
	Box largest_parent_box;
	// find the largest box the entity was in 
	for (auto leaf_index : ent_data.leaf_indices) {
		Box parent_box = leaf_data_[leaf_index].leaf_box;
		if (parent_box.GetVolume() > largest_parent_box.GetVolume()) {
			largest_parent_box = parent_box;
			largest_parent_node = leaf_index;
		}
		// if they dont intersect then the entity moved out of the leaf's box
		if (!CheckBoxIntersect(parent_box, ent_box)) {
			leaf_data_[leaf_index].entity_data.erase(handle);
		}
	}
	// go up tree_ until the entity is encompassed then reinsert
	u32 tree_node = leaf_data_[largest_parent_node].tree_index;
	while (!CheckBoxEncompass(largest_parent_box, ent_box)) {
		u32 i = 0;
		while (tree_[tree_node].leaf_indices[i] != largest_parent_node) {
			++i;
		}
		Octant child_oct{ i };
		largest_parent_box = GetParentBox(largest_parent_box, child_oct);
		tree_node = tree_[tree_node].previous;
	}
	// clear the old leaves and find and add the new ones
	ent_data.leaf_indices.clear();
	FindLeafData(ent_data.leaf_indices, tree_node, largest_parent_box, ent_box);
	AddToLeaf(handle, ent_box, ent_data.leaf_indices);
}

void PositionHandler::FindLeafData(unordered_set<u32>& leaf_indices, u32 const start,
	Box const pbox, Box const ent_box) const {
	// iterate over the octants of the given parent node and if it intersects
	// then continue down tree until reaching leaves with intersections
	auto& tree_node = tree_[start];
	for (u32 i = 0; i < tree_node.leaf_indices.size(); ++i) {
		auto next_i = tree_node.next_indices[i];
		auto leaf_i = tree_node.leaf_indices[i];
		Octant oct{ i };	// octants are just the bitset of i (0-7)
		// get Box for this octant
		Box OctantBox = GetOctantBox(pbox, oct);
		// check if the entity box intersects the octant
		bool intersect = CheckBoxIntersect(OctantBox, ent_box);

		if (next_i == 0 && intersect) {	// if it's a leaf then add it to the vector
			leaf_indices.insert(leaf_i);
		}
		// if it intersects, but we arent at a leaf yet we recurse
		if (next_i != 0 && intersect) {
			return FindLeafData(leaf_indices, next_i, OctantBox, ent_box);
		}
	}
}

PositionHandler::Box PositionHandler::GetOctantBox(Box const pbox, Octant const oct) const {
	// A giant gross switch for each octant 
	// remember pos is the center of the box and dims are the side lengths
	// and 0 represents the positive direction
	auto num = oct.to_ulong();
	Box result;
	result.dims = pbox.dims / 2.0f;
	switch (num) {
		// bitset(000) = pos x, pos y, pos z
	case 0:
		result.pos = pbox.pos + result.dims / 2.0f;
		break;
		// bitset(001) = neg x, pos y, pos z
	case 1:
		result.pos = pbox.pos + result.dims / 2.0f;
		result.pos.x -= result.dims.x;
		break;
		// 010
	case 2:
		result.pos = pbox.pos + result.dims / 2.0f;
		result.pos.y -= result.dims.y;
		break;
		// 011
	case 3:
		result.pos = pbox.pos - result.dims / 2.0f;
		result.pos.z += result.dims.z;
		break;
		// 100
	case 4:
		result.pos = pbox.pos + result.dims / 2.0f;
		result.pos.z -= result.dims.z;
		break;
		// 101
	case 5:
		result.pos = pbox.pos - result.dims / 2.0f;
		result.pos.y += result.dims.y;
		break;
		// 110
	case 6:
		result.pos = pbox.pos - result.dims / 2.0f;
		result.pos.x += result.dims.x;
		break;
		// 111
	case 7:
		result.pos = pbox.pos - result.dims / 2.0f;
		break;
	default:
		break;	// wont happen but compiler is bad
	}
	return result;
}

bool PositionHandler::CheckBoxIntersect(Box const box1, Box const box2) const {
	// for each axis we check if the distance from centers
	// is less than the sums of the half lengths of their sides
	for (int i = 0; i < 3; ++i) {
		auto distance = std::abs(box1.pos[i] - box2.pos[i]);
		auto span = (box1.dims[i] + box2.dims[i]) / 2.0f;
		if (distance > span) {
			// Boxes overlap along this axis
			return false;
		}
	}
	return true;
}

void PositionHandler::AddToLeaf(EntityID const handle, Box const ent_box,
	unordered_set<u32>& leaf_indices) {
	for (auto leaf_index : leaf_indices) {
		auto& leaf_node = leaf_data_[leaf_index];
		leaf_node.entity_data[handle] = ent_box;
		// if this is the limit we need to split octants which means adding to
		// both tree_ and leaf_data_ and forwarding the current nodes to the new ones
		if (leaf_node.entity_data.size() < max_entity_per_octant) { return; }
		else {
			// make new temp vector of new leaf indices for recursion
			// because too many might still overlap in the new octants
			// and we don't want to recurse over already finished leaf_indices
			unordered_set<u32> temp_leaf_indices;
			// add new TreeNode for new octant
			TreeNode new_node;
			new_node.previous = leaf_node.tree_index;
			tree_.push_back(new_node);
			u32 new_node_index = tree_.size() - 1;
			auto& parent_node = tree_[leaf_node.tree_index];
			// make a deep copy of leaf_node because we will overwrite it
			auto old_leaf = leaf_node;
			// make new leaves for each octant and update the new TreeNode's leaf_indices
			for (auto i = 0; i < parent_node.leaf_indices.size(); ++i) {
				LeafData new_leaf;
				new_leaf.tree_index = new_node_index;
				new_leaf.leaf_box = GetOctantBox(old_leaf.leaf_box, Octant(i));

				for (auto& [ent, box_] : old_leaf.entity_data) {
					// if an entity from the parent leaf is in this octant then add it
					if (CheckBoxIntersect(new_leaf.leaf_box, box_)) {
						new_leaf.entity_data[ent] = box_;
					}
				}
				// forward the next_index and overwrite old LeafData
				if (parent_node.leaf_indices[i] == leaf_index) {
					parent_node.next_indices[i] = new_node_index;
					leaf_node = new_leaf;
					tree_[new_node_index].leaf_indices[i] = leaf_index;
				}
				// for the other 7 we push new LeafData into leaf_data_
				// and add to the leaf_indices for this entity
				else {
					leaf_data_.push_back(new_leaf);
					u32 new_leaf_index = leaf_data_.size() - 1;
					tree_[new_node_index].leaf_indices[i] = new_leaf_index;
					// add to temp because we will be recursing
					temp_leaf_indices.insert(new_leaf_index);
				}
			}
			AddToLeaf(handle, ent_box, temp_leaf_indices);
			// now add the temp indices to the original
			leaf_indices.insert(temp_leaf_indices.begin(), temp_leaf_indices.end());
		}
	}
}

PositionHandler::Box PositionHandler::GetParentBox(Box const child_box, 
	Octant const child_oct) const {
	Box result;
	result.dims = child_box.dims * 2.0f;
	auto num = child_oct.to_ulong();
	switch (num) {
	case 0:
		result.pos = child_box.pos - child_box.dims / 2.0f;
		break;
	case 1:	// 001
		result.pos = child_box.pos - child_box.dims / 2.0f + child_box.dims.x;
		break;
	case 2:
		result.pos = child_box.pos - child_box.dims / 2.0f + child_box.dims.y;
		break;
	case 3:
		result.pos = child_box.pos + child_box.dims / 2.0f - child_box.dims.z;
		break;
	case 4:
		result.pos = child_box.pos - child_box.dims / 2.0f + child_box.dims.z;
		break;
	case 5:
		result.pos = child_box.pos + child_box.dims / 2.0f - child_box.dims.y;
		break;
	case 6:
		result.pos = child_box.pos + child_box.dims / 2.0f - child_box.dims.x;
		break;
	case 7:
		result.pos = child_box.pos + child_box.dims / 2.0f;
		break;
	}
	return result;
}

bool PositionHandler::CheckBoxEncompass(Box const box1, Box const box2) const {
	bool x_check = (box1.pos.x + box1.dims.x / 2 > box2.pos.x + box2.dims.x / 2) &&
		(box1.pos.x - box1.dims.x / 2 < box2.pos.x - box2.dims.x / 2);
	bool y_check = (box1.pos.y + box1.dims.y / 2 > box2.pos.y + box2.dims.y / 2) &&
		(box1.pos.y - box1.dims.y / 2 < box2.pos.y - box2.dims.y / 2);
	bool z_check = (box1.pos.z + box1.dims.z / 2 > box2.pos.z + box2.dims.z / 2) &&
		(box1.pos.z - box1.dims.z / 2 < box2.pos.z - box2.dims.z / 2);
	return x_check && y_check && z_check;
}

PositionHandler::Box PositionHandler::GetEntityBox(EntityID const handle) const {
	return index_.at(handle).entity_box;
}

vector<pair<EntityID const, PositionHandler::Box>> 
	PositionHandler::GetEntityBoxes(Box const& query_box)
{
	vector<pair<EntityID const, PositionHandler::Box>>  ent_boxes;
	unordered_set<u32> leaf_indices;
	FindLeafData(leaf_indices, 0, boundaries_, query_box);
	unordered_set<EntityID> already_added;
	for (auto leaf_index : leaf_indices) {
		auto leaf_data = leaf_data_[leaf_index];
		for (auto& entity_data : leaf_data.entity_data) {
			auto found_ent = already_added.find(entity_data.first);
			if (found_ent == already_added.end()) {
				ent_boxes.push_back(entity_data);
				already_added.emplace(entity_data.first);
			}
		}
	}
	return ent_boxes;
}

