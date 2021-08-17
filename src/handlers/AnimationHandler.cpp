#include "AnimationHandler.h"
#include "globals.h"

#include <cmath>
using std::vector, std::string, std::abs;

AnimationHandler::AnimationHandler(QuadHandler* qh, PositionHandler* ph) :
	quad_handler_(qh), pos_handler_(ph) {}

void AnimationHandler::WalkAnim(EntityHandler::ID const handle, QuadParams& qp,
								double const period, double const new_pos) {
	// for the walk animation info.previous refers the the spatial distance
	// through the cycle it was last update
	// info.state refers to the 0 to 1.0 progress through the animation

	// cycleLen is number of different frames in the animation
	// we subtract 1 because 0 is the idle animation
	int const cycleLen = qp.cols - 1;
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
		qp.col = info.active_quad;
		quad_handler_->SetActive(handle, qp); // sets this one to the active quad
	}
}

void AnimationHandler::Update(EntityHandler::ID const handle) {
	auto found = anim_data_.find(handle);
	if(found != anim_data_.end()){
		auto& ent_pos = pos_handler_->GetPos(handle);
		switch (found->second.active_anim) {
			case AnimType::PLAYERWALKUP: {
				QuadParams qp{
				"walkcycle/BODY_male.png",
				"quadshader.vs",
				"quadshader.fs",
				2 * ent_pos.width,
				2 * ent_pos.height,
				3,	//row
				4,	//rows
				0,	//column
				9	//columns
				};
				WalkAnim(handle, qp, 4.0, ent_pos.yPos);
				break;
			}

			case AnimType::PLAYERWALKDOWN: {
				QuadParams qp{
				"walkcycle/BODY_male.png",
				"quadshader.vs",
				"quadshader.fs",
				2 * ent_pos.width,
				2 * ent_pos.height,
				1,
				4,
				0,
				9
				};
				WalkAnim(handle, qp, 4.0, ent_pos.yPos);
				break;
			}

			case AnimType::PLAYERWALKLEFT: {
				QuadParams qp{
				"walkcycle/BODY_male.png",
				"quadshader.vs",
				"quadshader.fs",
				2 * ent_pos.width,
				2 * ent_pos.height,
				2,
				4,
				0,
				9
				};
				WalkAnim(handle, qp, 4.0, ent_pos.xPos);
				break;
			}

			case AnimType::PLAYERWALKRIGHT: {
				QuadParams qp{
				"walkcycle/BODY_male.png",
				"quadshader.vs",
				"quadshader.fs",
				2 * ent_pos.width,
				2 * ent_pos.height,
				0,
				4,
				0,
				9
				};
				WalkAnim(handle, qp, 4.0, ent_pos.xPos);
				break;
			}
		}
	}
}

void AnimationHandler::SetActive(EntityHandler::ID const handle, AnimType tp)
{
	// TODO: error catching
	anim_data_[handle].active_anim = tp;
}

void AnimationHandler::Add(EntityHandler::ID const handle, 
						   vector<AnimType> types)
{
	auto found = anim_data_.find(handle);
	if (found != anim_data_.end()) {
		for (auto& type : types) {
			found->second.anims.emplace(type);
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

void AnimationHandler::Remove(EntityHandler::ID const handle, 
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
		string err = "cannot remove entity " + handle + " from animations";
		throw std::runtime_error(err);
	}
}