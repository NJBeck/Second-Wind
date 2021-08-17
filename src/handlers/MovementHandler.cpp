#include "handlers/MovementHandler.h"
#include "globals.h"

#include <cmath>
#include <string>
#include <stdexcept>

using namespace std;

MovementHandler::MovementHandler(PositionHandler* ph, Timer* t)
	: pos_handler_(ph), timer_(t)
{}

void MovementHandler::Add(uint64_t handle, Velocity vector)
{
	velocities_[handle] = vector;
}
void MovementHandler::AddVelocity(EntityHandler::ID const handle, 
								  Velocity const vec) {
	auto it = velocities_.find(handle);
	if (it != velocities_.end()) {
		Velocity& ent_vector = velocities_[handle];
		ent_vector.xVector += vec.xVector;
		ent_vector.yVector += vec.yVector;
	}
	else {
		string err = handle + " not found when trying to add Velocity";
		throw runtime_error(err);
	}

	
}

void MovementHandler::SetVelocity(EntityHandler::ID const handle,
								  Velocity const vec)
{
	velocities_[handle] = { vec.xVector, vec.yVector };
}

void MovementHandler::Remove(EntityHandler::ID const handle)
{
	velocities_.erase(handle);
}

void MovementHandler::Update() const
{
	float timestep = timer_->lastFrameTime() / 1000;
	for (auto& ent_vector : velocities_) {
		double const& xvel = ent_vector.second.xVector;
		double const& yvel = ent_vector.second.yVector;
		double x_displace = xvel * timestep;
		double y_displace = yvel * timestep;

		pos_handler_->Move(ent_vector.first, { x_displace, y_displace });

	}
}
