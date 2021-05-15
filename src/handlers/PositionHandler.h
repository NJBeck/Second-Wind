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
	struct EntPosDim {
		entity::ID ID;
		Dimensions dimensions;
		Pos pos;

	};
	struct NodeInfo {
		uint32_t index;	// index of leaf in quadtree_ vector
		// width/height/position of that leaf's quadrant
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
		uint32_t next;	// index of first child in vector (0 if leaf)
		std::set<EntPosDim> entities;
	};


	void Query(Pos const posn, Dimensions const dims, NodeInfo& current_node);
	std::array<bool, 4> FindQuad(Pos const entity_pos, 
								 Dimensions const entity_dim, 
								 Pos const quadrant_pos, 
								 Dimensions const quadrant_dim);
	bool IntervalTest(double const start1, double const end1, 
					  double const start2, double const end2);


	uint32_t entity_max_;	// max num of entities before it splits
	double quad_limit_;	// recursion depth limit for the tree
	NodeInfo origin_;	// starting node to be iterated on by Query
	std::set<NodeInfo> query_result_;	// stores the results from Query
	std::vector<ListNode> quadtree_;

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
