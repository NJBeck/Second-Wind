#pragma once
#include "PositionHandler.h"
#include "utility.h"


class MovementHandler
{
public:
	typedef glm::vec3 Velocity;
	MovementHandler(PositionHandler& ph, Timer& tm) :
		timer_(tm), pos_handler_(ph) {}
	// glm vec3 are zero initialized
	void Add(EntityID const, Velocity const);
	// add xVelocity and yVelocity to entity
	void AddVelocity(EntityID const, Velocity const);
	// sets the Velocity of entity
	void SetVelocity(EntityID const, Velocity const);
	// remove an entity from this system
	void Remove(EntityID const handle);
	// update all the positions of moving objects based on their velocities
	void Update();
private:
	std::unordered_map<EntityID, Velocity> index_;
	Timer& timer_;
	PositionHandler& pos_handler_;
};

