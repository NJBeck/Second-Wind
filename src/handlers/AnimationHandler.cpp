#include "AnimationHandler.h"
#include "globals.h"

#include <cmath>
using std::vector, std::string, std::abs;

void AnimationHandler::_WalkAnim(uint64_t const handle, QuadParams& qp, double const period, double const newPos) {
	int const cycleLen = qp.cols - 1;	// cycleLen is number of different frames in the animation
	int const row = qp.row;				// row of the spritesheet for this animation
										// period distance over which a full cycle takes

	AnimInfo& _info = animData[handle];	// get the stored animations for this entity
	// finding how far we've moved since last update
	double displacement = abs(newPos - _info.previous);
	_info.previous = newPos;	// store the new position

	uint32_t prevActive = _info.activeQuad; // save to check if it changed later

	// if no movement then active quad should be idle position
	if (displacement < 0.000001) {
		_info.activeQuad = 0;
		_info.state = 0.0;	// state saves the progress through the animation cycle
	}
	else {
		// objective is to find how far through the period we are
		// then map that to a quad in the animation cycle

		// get the new progress by adding the old progress and the displacement
		// use modulo over the period to handle the rollover
		_info.state = fmod((_info.state + displacement), period);
		// frequency = 0.5 which is how frequently over the period the animation updates
		double frequency = period / cycleLen;
		// round up to the nearest frequency value to find the index for the active quad
		// add 1 because 0 is just for the idle animation
		_info.activeQuad = static_cast<unsigned>(_info.state / frequency) + 1;
	}
	qp.col = _info.activeQuad;

	if (prevActive != _info.activeQuad) {
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
				0,		// row of quad's texture in spritesheet
				4,
				0,
				9
				};
				_WalkAnim(handle, qp, 4.0, globals::posHandler.GetPos(handle).yPos);
				globals::quadHandler.SetActive(handle, qp);
				break;
			}

			case AnimType::PLAYERWALKDOWN: {
				QuadParams qp{
				"walkcycle/BODY_male.png",	// filepath for image relative to /data/ folder
				"quadshader.vs",	// file path for the vertex shader source
				"quadshader.fs",
				2 / static_cast<float>(globals::renderer.cam.width),	// screenspace width of quad
				2 / static_cast<float>(globals::renderer.cam.height),
				2,		// row of quad's texture in spritesheet
				4,
				0,
				9
				};
				_WalkAnim(handle, qp, 4.0, globals::posHandler.GetPos(handle).yPos);
				globals::quadHandler.SetActive(handle, qp);
				break;
			}

			case AnimType::PLAYERWALKLEFT: {
				QuadParams qp{
				"walkcycle/BODY_male.png",	// filepath for image relative to /data/ folder
				"quadshader.vs",	// file path for the vertex shader source
				"quadshader.fs",
				2 / static_cast<float>(globals::renderer.cam.width),	// screenspace width of quad
				2 / static_cast<float>(globals::renderer.cam.height),
				1,		// row of quad's texture in spritesheet
				4,
				0,
				9
				};
				_WalkAnim(handle, qp, 4.0, globals::posHandler.GetPos(handle).xPos);
				globals::quadHandler.SetActive(handle, qp);
				break;
			}

			case AnimType::PLAYERWALKRIGHT: {
				QuadParams qp{
				"walkcycle/BODY_male.png",	// filepath for image relative to /data/ folder
				"quadshader.vs",	// file path for the vertex shader source
				"quadshader.fs",
				2 / static_cast<float>(globals::renderer.cam.width),	// screenspace width of quad
				2 / static_cast<float>(globals::renderer.cam.height),
				3,		// row of quad's texture in spritesheet
				4,
				0,
				9
				};
				_WalkAnim(handle, qp, 4.0, globals::posHandler.GetPos(handle).xPos);
				globals::quadHandler.SetActive(handle, qp);
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
