// TODO: logging and error handling
#include "PositionHandler.h"

using std::vector, std::unordered_set, std::bitset;

PositionHandler::PositionHandler() {
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
}

void PositionHandler::Move(EntityID const handle, Dir const move_vec) {
	auto& entity_data = index_[handle];
	entity_data.entity_box.pos += move_vec;
	MoveHelper(handle, entity_data);
}

void PositionHandler::MoveHelper(EntityID handle, EntityData& entity_data) {

	// go up tree to find first TreeNode that encompasses the entity
	TreeNode highest_node;
	Box largest_node_box;
	for (auto leaf_index : entity_data.leaf_indices) {
		// if it's no longer in this leaf's octant we remove it
		auto& leaf_data = leaf_data_[leaf_index];
		if (!CheckBoxIntersect(leaf_data.leaf_box, entity_data.entity_box)) {
			leaf_data.entity_data.erase(handle);
		}
		else {
			// if the leaf's box is bigger it must be higher up the tree
			if (leaf_data.leaf_box.volume > largest_node_box.volume) {
				// check if the parent Box of the leaf encompasses the entity
				Box parent_box = GetParentBox(leaf_index);
				if (CheckBoxEncompass(parent_box, entity_data.entity_box)) {
					// if it does then we do the usual addition
					unordered_set<u32> leaf_indices;
					FindLeafData(leaf_indices, leaf_data.tree_index,
						parent_box, entity_data.entity_box);
					entity_data.leaf_indices = std::move(leaf_indices);
				}
			}
			// if leaf box isn't bigger or doesnt encompass then we recurse up the tree_
			MoveHelper(handle, entity_data);
		}
	}
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
		result.pos = pbox.pos + result.dims / 2.0f - result.dims.x;
		break;
		// 010
	case 2:
		result.pos = pbox.pos + result.dims / 2.0f - result.dims.y;
		break;
		// 011
	case 3:
		result.pos = pbox.pos - result.dims / 2.0f + result.dims.z;
		break;
		// 100
	case 4:
		result.pos = pbox.pos + result.dims / 2.0f - result.dims.z;
		break;
		// 101
	case 5:
		result.pos = pbox.pos - result.dims / 2.0f + result.dims.y;
		break;
		// 110
	case 6:
		result.pos = pbox.pos - result.dims / 2.0f + result.dims.x;
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
		if (distance < span) {
			// Boxes overlap along this axis
			return true;
		}
	}
	return false;
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

PositionHandler::Box PositionHandler::GetParentBox(u32 const leaf_index) const {
	auto& leaf_data = leaf_data_[leaf_index];
	auto leaf_box = leaf_data.leaf_box;
	auto& tree_node = tree_[leaf_data.tree_index];
	Octant leaf_octant; // the octant of the TreeNode this leaf is from
	// iterate over leaf_indices to find which octant it's in
	for (auto i = 0; i < tree_node.leaf_indices.size(); ++i) {
		if (tree_node.leaf_indices[i] == leaf_index) {
			leaf_octant = Octant(i);
			break;
		}
	}
	Box result;
	result.dims = leaf_box.dims * 2.0f;
	auto num = leaf_octant.to_ulong();
	switch (num) {
	case 0:
		result.pos = leaf_box.pos - leaf_box.dims / 2.0f;
	case 1:	// 001
		result.pos = leaf_box.pos - leaf_box.dims / 2.0f + leaf_box.dims.x;
	case 2:
		result.pos = leaf_box.pos - leaf_box.dims / 2.0f + leaf_box.dims.y;
	case 3:
		result.pos = leaf_box.pos + leaf_box.dims / 2.0f - leaf_box.dims.z;
	case 4:
		result.pos = leaf_box.pos - leaf_box.dims / 2.0f + leaf_box.dims.z;
	case 5:
		result.pos = leaf_box.pos + leaf_box.dims / 2.0f - leaf_box.dims.y;
	case 6:
		result.pos = leaf_box.pos + leaf_box.dims / 2.0f - leaf_box.dims.x;
	case 7:
		result.pos = leaf_box.pos + leaf_box.dims / 2.0f;
	}
	return result;
}

bool PositionHandler::CheckBoxEncompass(Box const box1, Box const box2) const {
	bool x_check = (box1.pos.x + box1.dims.x / 2 > box2.pos.x + box2.dims.x / 2) &&
		(box1.pos.x - box1.dims.x / 2 > box2.pos.x + box2.dims.x / 2);
	bool y_check = (box1.pos.y + box1.dims.y / 2 > box2.pos.y + box2.dims.y / 2) &&
		(box1.pos.y - box1.dims.y / 2 > box2.pos.y + box2.dims.y / 2);
	bool z_check = (box1.pos.z + box1.dims.z / 2 > box2.pos.z + box2.dims.z / 2) &&
		(box1.pos.z - box1.dims.z / 2 > box2.pos.z + box2.dims.z / 2);
	return x_check && y_check && z_check;
}