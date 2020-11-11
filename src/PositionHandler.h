#pragma once
// handles the storage of world positions of all entities
// entity positions on each axis are stored in their own vectors
// vectors are sorted from lowest to highest
// index to component info of each entity is stored in a hashmap

//TODO:	defer sorting perhaps,
//		accumulate hashmap update during insertion/position updates,
//		implement entity deletion/garbage collection,
//		make helper function for add() that works on a vector individually

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

struct Indices {
	unsigned long xIndex;
	unsigned long yIndex;
};

class PositionHandler {
	// vectors of {entity, position} pairs sorted by position in increasing order
	std::vector<EntityPos> xPositions;
	std::vector<EntityPos> yPositions;
	// maps entity to position
	std::unordered_map<unsigned long, Indices> Index;

	// binary search for the index of first element in the vector that is larger than the input
	// (helper function for Entites in range) returns -1 if none is larger
	long FirstPos(double, std::vector<EntityPos>);
	// helper function: returns entities which are at least as big as start and less than stop
	std::vector<unsigned long> EntitiesInRange(double start, double stop, std::vector<EntityPos> positions);
public:
	// returns the entites with Pos between left/right and top/bottom
	// returns sorted by Y from top to bottom
	std::vector<unsigned long> EntitiesInRanges(double left, double right, double bottom, double top);
	// returns Pos of entity from hashmap
	Pos GetPos(unsigned long);
	// changes entity position by Pos amount
	void ChangePos(unsigned long, Pos);
	// adds entity with this pos to handler
	void add(unsigned long, Pos);
	// removes this entity from handler
	void del(unsigned long);

};
