#pragma once
#include <unordered_map>

// data for direction and speed of entity
struct velocity {
	// xDirection and yDirection should be normalized so x^2 + y^2 = 1
	double xDirection;
	double yDirection;
	double speed;
};

class MovementHandler
{
	// map for currently moving objects
	std::unordered_map<uint64_t, velocity> velocities;
	// map of objects speed attributes
	std::unordered_map<uint64_t, double> speedAttribute;
public:
	//register and entity with a certain speed attribute
	// also use this to change an entity's speed attribute
	void add(uint64_t handle, double);
	// add xVelocity and yVelocity to 
	void addVelocity(uint64_t, double xDirection, double yDirection);
	// remove an entity from this system
	void remove(uint64_t handle);
	// update all the positions of moving objects based on their velocities
	void update();
};

