#include "handlers/MovementHandler.h"
#include "globals.h"

#include <cmath>
#include <string>
#include <stdexcept>


void MovementHandler::Add(EntityID handle, Velocity vector)
{
	index_[handle] = vector;
}
void MovementHandler::AddVelocity(EntityID const handle, 
								  Velocity const vec) {
	index_[handle] += vec;
}

void MovementHandler::SetVelocity(EntityID const handle,
								  Velocity const vec)
{
	index_[handle] = vec;
}

void MovementHandler::Remove(EntityID const handle)
{
	auto found = index_.find(handle);
	if (found == index_.end()) {
		utility::Log("MovementHandler", "removing entity", "not in handler");
	}
	else {
		index_.erase(found);
	}

}

void MovementHandler::Update()
{
	float timestep = timer_->lastFrameTime() / 1000;
	for (auto& ent_ : index_) {
		Velocity& ent_vec = ent_.second;
		Dir displacement = ent_vec * timestep;
		ent_vec += displacement;
		// update the position
		pos_handler_->Move(ent_.first, displacement);

	}
}
