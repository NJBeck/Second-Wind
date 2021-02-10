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

struct EntityPos {
	uint64_t ID;
	double Pos;
};

struct Pos {
	double xPos;
	double yPos;
};

enum class direction {
	x,
	y
};


class PositionHandler {
	// vectors of {entity, position} pairs sorted by position in increasing order
	std::vector<EntityPos> xPositions;
	std::vector<EntityPos> yPositions;
	// maps entity to index in relevant positions vector
	std::unordered_map<uint64_t, uint32_t> _xIndex;
	std::unordered_map<uint64_t, uint32_t> _yIndex;
	
	// binary search for the index of first element in the vector that is larger than the input
	// returns -1 if none is larger; direction selects whether to update X/Y directions
	long FirstPos(double const, direction const);
	// helper function: returns entities which are at least as big as start and less than stop from a vector of positions
	std::vector<uint64_t> _InRange(double const start, double const stop, direction const);
	// helper function for ChangePos
	void _changepos(uint64_t const, double const, direction const);
	// helper function for add
	void _addpos(uint64_t const, double const, direction const);
public:
	// returns the entites with Pos between left/right and top/bottom
	// returns sorted by Y from top to bottom
	std::vector<uint64_t> InRange(double const left, double const right, double const bottom, double const top);
	// returns Pos of entity from hashmap
	Pos GetPos(uint64_t const);
	// changes entity position by Pos amount
	void ChangePos(uint64_t const, Pos const);
	// adds entity with this pos to handler
	void add(uint64_t const, Pos const);
	// removes this entity from handler
	void del(uint64_t const);

};
