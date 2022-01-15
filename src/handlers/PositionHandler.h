#pragma once
// handles the storage of world positions of all entities
// entity positions on each axis are stored in their own vectors
// vectors are sorted from lowest to highest
// index to component info of each entity is stored in a hashmap

//TODO:	defer sorting perhaps,
//		accumulate hashmap update during insertion/position updates,
//		implement entity deletion/garbage collection
//		add function that returns entities with their positions in a certain range

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <set>

#include "globals.h"
// #include <ManagedContainer/ManagedContainer.h>


class PositionHandler {
public:
	struct Pos {
		double xPos;
		double yPos;
	};
	struct Dimensions {
		double width;
		double height;
	};
	struct Quad : public Pos, Dimensions{
	};

	PositionHandler(Quad boundaries);

	void Add(EntityID const handle, Quad const quad);

	void Remove(EntityID const handle);

	void Move(EntityID const handle, Pos const vec);

	Quad GetPos(EntityID const) const;

	struct EntityQuad {
		EntityID handle;
		Quad quad;
	};
	struct YDescendingOrder {
		bool operator() (EntityQuad const& lhs, EntityQuad const& rhs) const;
	};
	std::set<EntityQuad, YDescendingOrder> 
	EntitiesInArea(Quad const area) const;
private:
	enum Quadrants : uint8_t {
		NW = 0,
		NE,
		SE,
		SW
	};
	/*
	// HELPER FUNCTIONS
	*/

	// finds the indices for the leaves in leaf_data_ that the entity with the
	// desired quad would be found in
	// can start anywhere in the tree with a start hint
	std::unordered_set<uint32_t> Query(Quad const ent_quad, Quad init_quad,
									   uint32_t const start) const;

	// default query which starts at beginning of quadtree_
	std::unordered_set<uint32_t> Query(Quad const ent_quad) const;

	// inserts handle with Quad starting at a specific place in quadtree_
	// where the TreeNode has Quad boundary_quad and index start
	void Insert(EntityID const handle, Quad const ent_quad, 
				Quad const boundary_quad, uint32_t const start);

	// makes a new leaf in leaf_data_ which is the given quadrant of the parent
	uint32_t MakeNewLeaf(Quad const& parent, Quadrants, uint32_t parent_index);

	// returns which quadrants the entity intersects clockwise starting in NW
	std::array<bool, 4> FindQuad(Quad const entity_quad,
								 Quad const quadrant_quad) const;
	// tests whether 2 linear intervals intersect (used for FindQuad)
	bool IntervalTest(double const start1, double const end1, 
					  double const start2, double const end2) const;

	// tests whether the boundaries of 2 intersecting quadrilaterals cross
	// when quad1 is moved by a vector vec
	bool CrossBoundary(Quad const quad1, Pos const vec, 
					   Quad const quad2) const;

	// removes entity from the list of leaves given 
	// and reformats the trees as necessary
	void RemoveFromLeaf(EntityID const handle, 
						std::unordered_set<uint32_t> const& leaf_indices);
	

	//// calculates the position, width, height of the quadrant from
	//// the sequence of quadrants in the tree that specify it
	//Quad CalculateQuadrant(std::vector<Quadrants>&);

	// calculates the Quad of the given quadrant of the given quad q
	Quad QuadOfQuadrant(Quad const q, Quadrants const) const;

	// finds which quadrant of the parent the child is
	// (supposing it is infact one of the quadrants)
	Quadrants QuadrantOfQuad(Quad const parent, Quad const child) const;

	// calculates the Quad of the parent to a quadrant
	Quad ParentQuad(Quad const q, Quadrants const) const;

	// finds which quadrant in the parent node the child is
	Quadrants QuadrantOfParent(uint32_t const child) const;

	// tests if the inner quad is fully contained in the outer
	bool FullyContained(Quad const inner, Quad const outer) const;
	/*
	// END OF HELPER FUNCTIONS
	*/
	/*
	// DATA MEMBERS
	*/
	struct TreeNode {
		uint32_t previous;	// index of parent node in quadtree_
		// indices for each sub quadrants next node in quadtree_
		// in NW, NE, SE, SW order
		std::array<uint32_t, 4> next_indices; 
		// holds the indices for the 4 quadrants in leaf_data_ (if leaves)
		// in NW, NE, SE, SW order
		std::array<uint32_t, 4> leaf_indices;
	};
	std::vector<TreeNode> quadtree_;

	struct LeafData {
		std::unordered_set<EntityID> entities; //entities in this leaf
		Quad quad;	// the size and position of the quadrant
		uint32_t quadtree_index;	// index to the quadtree node for this leaf
		// need to know quadrant in the parent quadtree node because it has 4
		Quadrants quadrant;	
	};
	std::vector<LeafData> leaf_data_;

	struct EntityData {
		// stores the indices to the leaves in leaf_data_ that the entity is in
		std::unordered_set<uint32_t> indices;
		// stores position and size info of entity
		Quad quad;
	};
	// maps an entity to the indices of the leaves in leaf_data_ it's in
	std::unordered_map<EntityID, EntityData> index_;

	uint32_t const entity_max_;	// max num of entities before it splits
	double const resolution_;	// recursion depth limit for the tree
	Quad const map_boundaries_; // the boundaries for the map
};


