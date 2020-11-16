#include "MovementHandler.h"
#include "globals.h"
#include <cmath>

void MovementHandler::add(unsigned long handle, double speed)
{
	speedAttribute[handle] = speed;
}
void MovementHandler::addVelocity(unsigned long handle, double xDir, double yDir) {
	//if (xDir == 0.0 && yDir == 0.0) {
	//	velocities.erase(handle);
	//}
	//else {
	//	double normalizer = std::sqrt(xDir * xDir + yDir * yDir);
	//	velocities[handle] = {xDir / normalizer, yDir / normalizer, speedAttribute[handle]};
	//}
	auto it = velocities.find(handle);
	if (it != velocities.end()) {
		velocity& old = velocities[handle];
		old.xDirection += xDir;
		old.yDirection += yDir;
	}
	else {
		velocities[handle] = { xDir, yDir, speedAttribute[handle] };
	}

	
}

void MovementHandler::remove(unsigned long handle)
{
	velocities.erase(handle);
	speedAttribute.erase(handle);
}

void MovementHandler::update()
{
	float timestep = globals::globalTimer.lastFrameTime() / 1000;
	for (auto it = velocities.begin(); it != velocities.end();) {
		double& xvel = it->second.xDirection;
		double& yvel = it->second.yDirection;
		// check to see if object is moving by seeing if it has a noticeable x or y velocity
		if ((std::abs(xvel) > 0.0001) || (std::abs(yvel) > 0.0001)) {
			double normalizer = std::sqrt(xvel * xvel + yvel * yvel);
			double xDisplacement = it->second.xDirection * timestep * it->second.speed / normalizer;
			double yDisplacement = it->second.yDirection * timestep * it->second.speed / normalizer;
			globals::posHandler.ChangePos(it->first, { xDisplacement, yDisplacement });
			++it;
		}
		// if there's no motion then we remove it from the velocities map
		else {
			it = velocities.erase(it);
		}

	}
}
