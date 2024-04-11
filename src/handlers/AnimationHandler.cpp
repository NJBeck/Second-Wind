#include "AnimationHandler.h"
#include "globals.h"

#include <cmath>
using std::vector, std::string, std::abs, std::unordered_map;

const unordered_map<AnimationHandler::AnimType,
					vector<QuadHandler::QuadParams>> 
	AnimationHandler::anim_quads_ = {
	{AnimationHandler::AnimType::PLAYERWALKDOWN,{
		{ImageHandler::Image::WALK_BODY_MALE, 1, 0}, 
		{ImageHandler::Image::WALK_BODY_MALE, 1, 1},
		{ImageHandler::Image::WALK_BODY_MALE, 1, 2},
		{ImageHandler::Image::WALK_BODY_MALE, 1, 3},
		{ImageHandler::Image::WALK_BODY_MALE, 1, 4},
		{ImageHandler::Image::WALK_BODY_MALE, 1, 5},
		{ImageHandler::Image::WALK_BODY_MALE, 1, 6},
		{ImageHandler::Image::WALK_BODY_MALE, 1, 7},
		{ImageHandler::Image::WALK_BODY_MALE, 1, 8}}
	},
	{AnimationHandler::AnimType::PLAYERWALKUP,{
		{ImageHandler::Image::WALK_BODY_MALE, 3, 0},
		{ImageHandler::Image::WALK_BODY_MALE, 3, 1},
		{ImageHandler::Image::WALK_BODY_MALE, 3, 2},
		{ImageHandler::Image::WALK_BODY_MALE, 3, 3},
		{ImageHandler::Image::WALK_BODY_MALE, 3, 4},
		{ImageHandler::Image::WALK_BODY_MALE, 3, 5},
		{ImageHandler::Image::WALK_BODY_MALE, 3, 6},
		{ImageHandler::Image::WALK_BODY_MALE, 3, 7},
		{ImageHandler::Image::WALK_BODY_MALE, 3, 8}}
	},
	{AnimationHandler::AnimType::PLAYERWALKLEFT,{
		{ImageHandler::Image::WALK_BODY_MALE, 2, 0},
		{ImageHandler::Image::WALK_BODY_MALE, 2, 1},
		{ImageHandler::Image::WALK_BODY_MALE, 2, 2},
		{ImageHandler::Image::WALK_BODY_MALE, 2, 3},
		{ImageHandler::Image::WALK_BODY_MALE, 2, 4},
		{ImageHandler::Image::WALK_BODY_MALE, 2, 5},
		{ImageHandler::Image::WALK_BODY_MALE, 2, 6},
		{ImageHandler::Image::WALK_BODY_MALE, 2, 7},
		{ImageHandler::Image::WALK_BODY_MALE, 2, 8}}
	},
	{AnimationHandler::AnimType::PLAYERWALKRIGHT,{
		{ImageHandler::Image::WALK_BODY_MALE, 0, 0},
		{ImageHandler::Image::WALK_BODY_MALE, 0, 1},
		{ImageHandler::Image::WALK_BODY_MALE, 0, 2},
		{ImageHandler::Image::WALK_BODY_MALE, 0, 3},
		{ImageHandler::Image::WALK_BODY_MALE, 0, 4},
		{ImageHandler::Image::WALK_BODY_MALE, 0, 5},
		{ImageHandler::Image::WALK_BODY_MALE, 0, 6},
		{ImageHandler::Image::WALK_BODY_MALE, 0, 7},
		{ImageHandler::Image::WALK_BODY_MALE, 0, 8}}
	}
};

void AnimationHandler::WalkAnim(EntityID const handle, 
								QuadHandler::QuadParams& qp,
								double const period, double const new_pos) {
	// for the walk animation info.previous refers the the spatial distance
	// through the cycle it was last update
	// info.state refers to the 0 to 1.0 progress through the animation

	// cycleLen is number of different frames in the animation
	// we subtract 1 because 0 is the idle animation
	int const cycleLen = 8;
	// animation selects active quad in row based on how far entity has traveled

	AnimInfo& info = anim_data_[handle];
	auto prevActive = info.active_quad; // save to check if it changed later
	// finding how far we've moved since last update
	double displacement = abs(new_pos - info.previous);
	info.previous = new_pos;	// store the new position

	// if no movement then active quad should be idle position
	if (displacement < 0.000001) {
		info.active_quad = 0;
		info.state = 0.0;	// state saves the progress through the animation cycle
	}
	else {
		// objective is to find how far through the period we are
		// then map that to a quad in the animation cycle

		// get the new progress by adding the old progress and the displacement
		// use modulo over the period to handle the rollover
		info.state = fmod((info.state + displacement), period);
		// frequency = how frequently over the period the animation updates
		double frequency = period / cycleLen;
		// round up to the nearest multiple of frequency for index to active
		// add 1 because 0 is just for the idle animation
		info.active_quad = static_cast<uint32_t>(info.state / frequency) + 1;
	}

	if (prevActive != info.active_quad) {
		qp.columns = info.active_quad;
		quad_handler_.SetActive(handle, qp); // sets this one to the active quad
	}
}


void AnimationHandler::Update(EntityID const handle) {
	auto found = anim_data_.find(handle);
	if(found != anim_data_.end()){
		auto& ent_pos = pos_handler_.GetEntityBox(handle).pos;
		switch (found->second.active_anim) {
			case AnimType::PLAYERWALKUP: {
				QuadHandler::QuadParams qp{
					ImageHandler::Image::WALK_BODY_MALE,
					3, // row
					0  // column
				};
				WalkAnim(handle, qp, 4.0, ent_pos.y);
				break;
			}

			case AnimType::PLAYERWALKDOWN: {
				QuadHandler::QuadParams qp{
					ImageHandler::Image::WALK_BODY_MALE,
					1,
					0
				};
				WalkAnim(handle, qp, 4.0, ent_pos.y);
				break;
			}

			case AnimType::PLAYERWALKLEFT: {
				QuadHandler::QuadParams qp{
					ImageHandler::Image::WALK_BODY_MALE,
					2,
					0
				};
				WalkAnim(handle, qp, 4.0, ent_pos.x);
				break;
			}

			case AnimType::PLAYERWALKRIGHT: {
				QuadHandler::QuadParams qp{
					ImageHandler::Image::WALK_BODY_MALE,
					0,
					0
				};
				WalkAnim(handle, qp, 4.0, ent_pos.x);
				break;
			}
		}
	}
}

void AnimationHandler::SetActive(EntityID const handle, AnimType tp)
{
	// TODO: error catching
	anim_data_[handle].active_anim = tp;
}

void AnimationHandler::Add(EntityID const handle, 
						   vector<AnimType> types)
{
	auto found = anim_data_.find(handle);
	if (found != anim_data_.end()) {
		for (auto& type : types) {
			found->second.anims.emplace(type);
			quad_handler_.Add(handle, anim_quads_.at(type));
		}
		found->second.active_anim = types[0];
	}
	else {
		AnimInfo info;
		for (auto& type : types) {
			info.anims.emplace(type);
		}
		info.active_anim = types[0];
		anim_data_[handle] = info;
	}

}

void AnimationHandler::Remove(EntityID const handle, 
							  vector<AnimType> const types) {
	auto found = anim_data_.find(handle);
	if (found != anim_data_.end()) {
		auto& ent_anims = found->second.anims;
		for (auto& anim : types) {
			ent_anims.erase(anim);
		}
		if (ent_anims.size() == 0) anim_data_.erase(handle);
	}
	else {
		string err = "cannot remove entity " + std::to_string(handle) + 
					 " from animation handler";
		throw std::runtime_error(err);
	}
}