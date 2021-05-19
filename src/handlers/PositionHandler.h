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
#include <set>
#include <array>

#include "entities/entity.h"

// entities are sorted by descending y + height value then x + width then ID
bool operator<(QuadTree::EntPosDim& const, QuadTree::EntPosDim& const);
bool operator<(QuadTree::NodeInfo& const, QuadTree::NodeInfo& const);

struct Pos {
	double xPos;
	double yPos;
};
struct Dimensions {
	float width;
	float height;
};
class QuadTree {
public:
	enum class Quadrants : uint8_t {
		NW = 0,
		NE,
		SE,
		SW,
		NONE
	};
	struct PosData {
		entity::ID ID;
		Dimensions dimensions;
		Pos pos;

	};

	QuadTree(double const xmin, double const xmax, double const ymin,
		double const ymax);
	std::vector<uint32_t> Add(entity::ID handle, 
							  Pos const position, 
					          Dimensions const dim);
	friend class PositionHandler;
private:

	// the boundaries for the map
	double x_min_;
	double x_max_;
	double y_min_;
	double y_max_;

	struct ListNode {
		uint32_t next;	// index of first child in quadtree_ (0 if leaf)
		uint32_t index;	// index of data for the quadrant in data_
	};

	struct DataNode {
		std::vector<Quadrants> directions;
		std::set<uint32_t> entities;
	};


	void Query(Pos const posn, Dimensions const dims, uint32_t current_index);
	// returns which quadrants the entity intersects clockwise starting in NW
	std::set<uint32_t> query_result_;	// stores the results from Query

	std::array<bool, 4> FindQuad(Pos const entity_pos, 
								 Dimensions const entity_dim, 
								 Pos const quadrant_pos, 
								 Dimensions const quadrant_dim);
	// tests whether 2 linear intervals intersect (used for FindQuad)
	bool IntervalTest(double const start1, double const end1, 
					  double const start2, double const end2);


	uint32_t const entity_max_;	// max num of entities before it splits
	uint32_t const quad_limit_;	// recursion depth limit for the tree
	std::vector<ListNode> quadtree_;
	std::vector<DataNode> leaf_data_;
	std::vector<PosData> pos_data_;
};

class PositionHandler {
public:

	// returns Pos of entity from hashmap
	Pos GetPos(entity::ID const);
	// changes entity position by x_vec in x direction/y_vec in y direction
	void ChangePos(entity::ID const, double const x_vec, double const y_vec);
	// adds entity with this pos to handler
	void Add(entity::ID const handle, Pos const pos, Dimensions const dim);
	// removes this entity from handler
	void Remove(entity::ID const);
private:
	struct PosData {
		Dimensions dimensions;
		Pos pos;
		// indices for leaves in quadtree where entity is located
		std::set<uint32_t> quadtree_indices;
	};
	std::unordered_map<entity::ID, PosData> map_;
	QuadTree tree_;

};
