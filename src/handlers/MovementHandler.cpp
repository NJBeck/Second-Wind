#include "handlers/MovementHandler.h"
#include "globals.h"

#include <cmath>
#include <string>
#include <stdexcept>

using std::vector;

void MovementHandler::AddHandle(EntityID handle, Velocity vector)
{
	index_[handle] = vector;
}
void MovementHandler::AddVelocity(EntityID const handle, 
								  Velocity const vec) {
	auto found_ent = index_.find(handle);
	if (found_ent == index_.end()) {
		AddHandle(handle, {});
	}
	auto& ent_vec = index_[handle];
	ent_vec = AddVelocities(ent_vec, vec);
}

void MovementHandler::SetVelocity(EntityID const handle,
								  Velocity const vec)
{
	index_[handle] = vec;
}

void MovementHandler::AddDir(EntityID const handle, Dir const dir)
{
	auto found_ent = index_.find(handle);
	if (found_ent == index_.end()) {
		AddHandle(handle, { glm::normalize(dir), glm::length(dir) });
	}
	else {
		index_[handle].dir += dir;
	}
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
	float timestep = timer_.lastFrameTime() / 1000;
	vector<EntityID> to_remove;
	for (auto& ent_ : index_) {
		Velocity& ent_vec = ent_.second;
		// remove entities with 0 velocity
		if (ent_vec.speed * glm::length(ent_vec.dir) < 0.00001) {
			to_remove.push_back(ent_.first);
		}
		else {
			Dir displacement = glm::normalize(ent_vec.dir) * ent_vec.speed * timestep;
			pos_handler_.Move(ent_.first, displacement);
		}
	}
	for (auto ent : to_remove) {
		Remove(ent);
	}
}

MovementHandler::Velocity MovementHandler::AddVelocities(Velocity const v1, 
	Velocity const v2) const
{
	Velocity result;
	glm::vec3 vec1 = v1.dir * v1.speed;
	glm::vec3 vec2 = v2.dir * v2.speed;
	glm::vec3 res = vec1 + vec2;
	result.speed = glm::length(res);
	result.dir = glm::normalize(res);
	return result;
}
