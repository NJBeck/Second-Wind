#pragma once
#include "PositionHandler.h"
#include "utility.h"


class MovementHandler
{
public:
	struct Velocity {
		Dir dir{ 0.0 };
		float speed{ 0.0 };
		Velocity() = default;
		Velocity(Dir const dir_, float const speed_) : dir(dir_), speed(speed_) {}
	};
	MovementHandler(PositionHandler& ph, Timer& tm) :
		timer_(tm), pos_handler_(ph) {}
	// adds handle to index
	void AddHandle(EntityID const, Velocity const);
	// add velocity to entity (Adds it if not already in index)
	void AddVelocity(EntityID const, Velocity const);
	// sets the Velocity of entity
	void SetVelocity(EntityID const, Velocity const);
	// adds the given direction vector to the entity's current direction
	void AddDir(EntityID const, Dir const);
	// remove an entity from this system
	void Remove(EntityID const handle);
	// update all the positions of moving objects based on their velocities
	// removes those with 0 velocity
	void Update();
private:
	Velocity AddVelocities(Velocity const, Velocity const) const;
	std::unordered_map<EntityID, Velocity> index_;
	Timer& timer_;
	PositionHandler& pos_handler_;
};

