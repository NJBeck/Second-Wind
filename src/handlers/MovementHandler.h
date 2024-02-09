#pragma once
#include "PositionHandler.h"
#include "utility.h"


class MovementHandler
{
	typedef glm::vec3 Velocity;
	std::unordered_map<EntityID, Velocity> index_;
	std::unique_ptr<Timer>& timer_;
	std::unique_ptr<PositionHandler>& pos_handler_;
public:
	MovementHandler(std::unique_ptr<PositionHandler>& ph, std::unique_ptr<Timer>& tm):
		timer_(tm), pos_handler_(ph){}
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
};

