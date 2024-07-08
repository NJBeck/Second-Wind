#include "AnimationHandler.h"
#include "globals.h"

#include <cmath>
using std::vector, std::string, std::abs, std::unordered_map, std::unordered_set;

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

void AnimationHandler::Add(EntityID const handle, 
						   unordered_set<AnimType> types)
{
	auto& ent_data = index_[handle];
	for (auto& type : types) {
		ent_data.anims.emplace(type);
		quad_handler_.Add(handle, anim_quads_.at(type));
	}
}

void AnimationHandler::Remove(EntityID const handle, 
							  unordered_set<AnimType> const types) {
	auto found = index_.find(handle);
	if (found != index_.end()) {
		auto& ent_anims = found->second.anims;
		for (auto& anim : types) {
			ent_anims.erase(anim);
		}
		if (ent_anims.size() == 0) index_.erase(handle);
	}
	else {
		string err = "cannot remove entity " + std::to_string(handle) + 
					 " from animation handler";
		throw std::runtime_error(err);
	}
}

void AnimationHandler::ToUpdate(EntityID const handle) {
	to_update_.emplace(handle);
}

void AnimationHandler::Update()
{
	for (auto handle : to_update_) {
		auto& ent_data = index_[handle];
		auto anim_ty = ent_data.active_anim;
		switch (anim_ty) {
		case AnimType::PLAYERWALKUP: {
			auto ent_pos = pos_handler_.GetEntityBox(handle).pos;
			QuadHandler::QuadParams qp{
				ImageHandler::Image::WALK_BODY_MALE,
				3, // row
				0  // column
			};
			WalkAnim(handle, qp, 2.0, ent_pos.y);
			break;
		}
		case AnimType::PLAYERWALKDOWN: {
			auto ent_pos = pos_handler_.GetEntityBox(handle).pos;
			QuadHandler::QuadParams qp{
				ImageHandler::Image::WALK_BODY_MALE,
				1,
				0
			};
			WalkAnim(handle, qp, 2.0, ent_pos.y);
			break;
		}
		case AnimType::PLAYERWALKLEFT: {
			auto ent_pos = pos_handler_.GetEntityBox(handle).pos;
			QuadHandler::QuadParams qp{
				ImageHandler::Image::WALK_BODY_MALE,
				2,
				0
			};
			WalkAnim(handle, qp, 2.0, ent_pos.x);
			break;
		}
		case AnimType::PLAYERWALKRIGHT: {
			auto ent_pos = pos_handler_.GetEntityBox(handle).pos;
			QuadHandler::QuadParams qp{
				ImageHandler::Image::WALK_BODY_MALE,
				0,
				0
			};
			WalkAnim(handle, qp, 2.0, ent_pos.x);
			break;
		}
		}
	}
}

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

	AnimInfo& info = index_[handle];
	// save which step we're on to check if it changed later
	u32 prev_active = static_cast<u32>(cycleLen * info.state); 
	u32 new_active;
	// finding how far we've moved since last update
	double displacement = abs(new_pos - info.previous);
	info.previous = new_pos;	// store the new position

	// if no movement then active quad should be idle position
	if (displacement < 0.000001) {
		new_active = 0;
		info.state = 0.0;	// state saves the progress through the animation cycle
	}
	else {
		// objective is to find how far through the period we are
		// then map that to a quad in the animation cycle

		// get the new progress by adding the old progress and the displacement
		// use modulo over the period to handle the rollover
		info.state = fmod(((info.state * period) + displacement), period) / period;
		// add 1 because 0 is just for the idle animation
		new_active = static_cast<u32>(info.state * cycleLen) + 1;
	}

	if (prev_active != new_active) {
		qp.columns = new_active;
		quad_handler_.SetActive(handle, qp); // sets this one to the active quad
	}
}

void AnimationHandler::SetActive(EntityID const handle, AnimType tp)
{
	auto& ent_data = index_[handle];
	ent_data.active_anim = tp;
	ent_data.previous = 0;
	ent_data.state = 0;
	// set the current quad to the first one of that type
	quad_handler_.SetActive(handle, anim_quads_.at(tp)[0]);
}
