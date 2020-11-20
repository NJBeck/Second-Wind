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
	std::unordered_map<unsigned long, velocity> velocities;
	// map of objects speed attributes
	std::unordered_map<unsigned long, double> speedAttribute;
public:
	//register and entity with a certain speed attribute
	// also use this to change an entity's speed attribute
	void add(unsigned long handle, double);
	// add xVelocity and yVelocity to 
	void addVelocity(unsigned long, double xDirection, double yDirection);
	// remove an entity from this system
	void remove(unsigned long handle);
	// update all the positions of moving objects based on their velocities
	void update();
};

