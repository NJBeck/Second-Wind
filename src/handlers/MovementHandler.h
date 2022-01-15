#pragma once
#include "PositionHandler.h"
#include "utility.h"

#include <unordered_map>

// data for direction and speed of entity
struct Velocity {
	double xVector;
	double yVector;
};

class MovementHandler
{
	// map for currently moving objects
	std::unordered_map<EntityID, Velocity> velocities_;
	Timer* timer_;
public:
	PositionHandler* pos_handler_;
	MovementHandler(PositionHandler*, Timer*);
	// register and entity with a given Velocity
	void Add(uint64_t const handle, Velocity vec = { 0, 0 });
	// add xVelocity and yVelocity to entity
	void AddVelocity(EntityID const, Velocity const);
	// sets the Velocity of entity
	void SetVelocity(EntityID const, Velocity const);
	// remove an entity from this system
	void Remove(EntityID const handle);
	// update all the positions of moving objects based on their velocities
	void Update() const;
};

