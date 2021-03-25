#include "AnimationHandler.h"
#include "globals.h"

#include <cmath>
using std::vector, std::string, std::abs;

void AnimationHandler::_WalkAnim(uint64_t const handle, QuadParams& qp, double period, double const newPos) {
	int const cycleLen = qp.cols - 1;	// cycleLen is number of different frames in the animation
	// animation selects active quad in row based on how far entity has traveled
	// 0 is the idle quad that is selected when there has been no movement
	int const row = qp.row;				// row of the spritesheet for this animation

	AnimInfo& info = animData[handle];	// get the stored animations for this entity
	uint32_t prevActive = info.activeQuad; // save to check if it changed later
	// finding how far we've moved since last update
	double displacement = abs(newPos - info.previous);
	info.previous = newPos;	// store the new position

	// if no movement then active quad should be idle position
	if (displacement < 0.000001) {
		info.activeQuad = 0;
		info.state = 0.0;	// state saves the progress through the animation cycle
	}
	else {
		// objective is to find how far through the period we are
		// then map that to a quad in the animation cycle

		// get the new progress by adding the old progress and the displacement
		// use modulo over the period to handle the rollover
		info.state = fmod((info.state + displacement), period);
		// frequency = 0.5 which is how frequently over the period the animation updates
		double frequency = period / cycleLen;
		// round up to the nearest frequency value to find the index for the active quad
		// add 1 because 0 is just for the idle animation
		info.activeQuad = static_cast<uint32_t>(info.state / frequency) + 1;
	}

	if (prevActive != info.activeQuad) {
		qp.col = info.activeQuad;
		globals::quadHandler.SetActive(handle, qp); // sets this one to the active quad
	}
}

void AnimationHandler::update(uint64_t handle) {
	auto found = animData.find(handle);
	if(found != animData.end()){
		switch (found->second.activeAnim) {
			case AnimType::PLAYERWALKUP: {
				QuadParams qp{
				"walkcycle/BODY_male.png",	// filepath for image relative to /data/ folder
				"quadshader.vs",	// file path for the vertex shader source
				"quadshader.fs",
				2 / static_cast<float>(globals::renderer.cam.width),	// screenspace width of quad
				2 / static_cast<float>(globals::renderer.cam.height),
				3,		// row of quad's texture in spritesheet (counting from bottom up sheet up)
				4,
				0,
				9
				};
				_WalkAnim(handle, qp, 4.0, globals::posHandler.GetPos(handle).yPos);
				break;
			}

			case AnimType::PLAYERWALKDOWN: {
				QuadParams qp{
				"walkcycle/BODY_male.png",
				"quadshader.vs",
				"quadshader.fs",
				2 / static_cast<float>(globals::renderer.cam.width),
				2 / static_cast<float>(globals::renderer.cam.height),
				1,
				4,
				0,
				9
				};
				_WalkAnim(handle, qp, 4.0, globals::posHandler.GetPos(handle).yPos);
				break;
			}

			case AnimType::PLAYERWALKLEFT: {
				QuadParams qp{
				"walkcycle/BODY_male.png",
				"quadshader.vs",
				"quadshader.fs",
				2 / static_cast<float>(globals::renderer.cam.width),
				2 / static_cast<float>(globals::renderer.cam.height),
				2,
				4,
				0,
				9
				};
				_WalkAnim(handle, qp, 4.0, globals::posHandler.GetPos(handle).xPos);
				break;
			}

			case AnimType::PLAYERWALKRIGHT: {
				QuadParams qp{
				"walkcycle/BODY_male.png",
				"quadshader.vs",
				"quadshader.fs",
				2 / static_cast<float>(globals::renderer.cam.width),
				2 / static_cast<float>(globals::renderer.cam.height),
				0,
				4,
				0,
				9
				};
				_WalkAnim(handle, qp, 4.0, globals::posHandler.GetPos(handle).xPos);
				break;
			}
		}
	}
}

void AnimationHandler::SetActive(uint64_t handle, AnimType tp)
{
	animData[handle].activeAnim = tp;
}

void AnimationHandler::add(uint64_t handle, vector<AnimType> types, AnimType active)
{
	auto found = animData.find(handle);
	if (found != animData.end()) {
		for (auto& type : types) {
			found->second.anims.emplace(type);
		}
		found->second.activeAnim = active;
	}
	else {
		AnimInfo info;
		for (auto& type : types) {
			info.anims.emplace(type);
		}
		info.activeAnim = active;
		animData[handle] = info;
	}

}

void AnimationHandler::del(uint64_t)
{
}
