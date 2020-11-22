#pragma once
// handles the storage of world positions of all entities
// entity positions on each axis are stored in their own vectors
// vectors are sorted from lowest to highest
// index to component info of each entity is stored in a hashmap

//TODO:	defer sorting perhaps,
//		accumulate hashmap update during insertion/position updates,
//		implement entity deletion/garbage collection

#include <vector>
#include <unordered_map>

struct EntityPos {
	unsigned long ID;
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
	// maps entity to position
	std::unordered_map<unsigned long, unsigned long> _xIndex;
	std::unordered_map<unsigned long, unsigned long> _yIndex;

	// binary search for the index of first element in the vector that is larger than the input
	// returns -1 if none is larger; direction selects whether to update X/Y directions
	long FirstPos(double, direction);
	// helper function: returns entities which are at least as big as start and less than stop from a vector of positions
	std::vector<unsigned long> _InRange(double start, double stop, direction);
	// helper function for ChangePos
	void _changepos(unsigned long, double, direction);
	// helper function for add
	void _addpos(unsigned long, double, direction);
public:
	// returns the entites with Pos between left/right and top/bottom
	// returns sorted by Y from top to bottom
	std::vector<unsigned long> InRange(double left, double right, double bottom, double top);
	// returns Pos of entity from hashmap
	Pos GetPos(unsigned long);
	// changes entity position by Pos amount
	void ChangePos(unsigned long, Pos);
	// adds entity with this pos to handler
	void add(unsigned long, Pos);
	// removes this entity from handler
	void del(unsigned long);

};
