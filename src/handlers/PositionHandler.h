#pragma once
//	handles the storage of world positions of all entities in an octree
//	goals: log(n) insertion, constant look up, keep the tree as flat as possible
//	solution: tree is only a vector of indices to next nodes in tree
//	leaves have the indices to a vector (leaf_data_) which stores 
//	the position state info of objects in that area
//	constant access comes from a hashmap from entity handles to this LeafData directly

//TODO:	batch the functions
// add removal function
// add shrinking for tree_
// if max_entity_per_octant perfectly overlap in octant there may be infinite recursion
// might use my ManagedContainer for leaf_data_ and tree_

#include <vector>
#include <unordered_map>
#include <bitset>
#include <unordered_set>
#include <array>
#include <deque>
#include <set>
#include "glm/glm.hpp"

#include "globals.h"


class PositionHandler {
public:
	struct Box {
		Box() = default;
		Box(Pos const pos_) : pos(pos_) {}	
		Box(Pos const pos_, Dims const dims_, Dir const dir_): 
			pos(pos_), dims(dims_), dir(dir_) {}
		Pos pos = Pos(0.0f);		// position of object center
		Dims dims = Dims(0.0f);		// xyz lengths of sides
		Dir dir = Dir(0.0f);		// orientation of object (mostly unused for now)
		double GetVolume() { return dims.x * dims.y * dims.z; }
	};

	void Add(EntityID const, Box const);

	void Remove(EntityID const);

	void Move(EntityID const, Dir const);

	Box GetEntityBox(EntityID const) const;
	// gives the entities in the same octants that intersect the given box
	std::vector<std::pair<EntityID const, Box>> GetEntityBoxes(Box const&);

	PositionHandler(Box const boundaries);
private:
	Box boundaries_;	// boundaries of the map
	// xyz where 0 represents positive direction (x is the LSb)
	typedef std::bitset<3> Octant;

	struct TreeNode {
		u32 previous = 0;	// index of parent node in tree_
		// indices for each sub octant next node in tree_
		// numerical representation of Octant is the index in the array for its next
		// eg the positive X positive Y negative Z octant would be to_ulong(011) = 3
		// if value in array at that index is 0 then this is a leaf
		std::array<u32, 8> next_indices = {};
		// holds the indices for the octants in leaf_data_ (if leaves)
		// to_ulong(Octant) is the index in the array for the leaf
		std::array<u32, 8> leaf_indices = {};
	};
	std::vector<TreeNode> tree_;

	struct LeafData {
		std::unordered_map<EntityID, Box> entity_data;	// stores the position info
		u32 tree_index;	// index of leaf node in the tree_
		Box leaf_box;	// position data of the leaf's area to check intersections
	};
	std::vector<LeafData> leaf_data_;

	struct EntityData {
		std::unordered_set<u32> leaf_indices;	// indices in leaf_data_ for its position
		Box entity_box;
	};
	std::unordered_map<EntityID, EntityData> index_;

	u32 max_entity_per_octant = 5;	//max number of entites in an octant before it splits
	//*********helper functions*******************************************

	// from 'start' in tree_ it stores the leaf_data_ indices for
	// the given entity box within the given bounding box (parent octant)
	void FindLeafData(std::unordered_set<u32>& leaf_indices, u32 const start,
		Box const pbox, Box const ent_box) const;
	// finds the Pos and Dims of the given octant of the given Box
	Box GetOctantBox(Box const, Octant const) const;
	// Gives the box of the parent to the given octant
	Box GetParentBox(Box const child_box, Octant const child_oct) const;
	// check if the boxes intersect anywhere
	bool CheckBoxIntersect(Box const, Box const) const;
	// check if Box1 encompasses Box2
	bool CheckBoxEncompass(Box const, Box const) const;
	// put the entity with the given box into the given LeafData from leaf_data_
	// splits into sub octants when too many entities are in the octant
	void AddToLeaf(EntityID const, Box const ent_box,
		std::unordered_set<u32>& leaf_indices);
};

