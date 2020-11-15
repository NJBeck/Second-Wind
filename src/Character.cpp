#include "Character.h"
#include "globals.h"

#include <cmath>

Character::Character(QuadParams quad, double xPos, double yPos)
{
	// add the position and quad components
	globals::quadHandler.add(handle, quad);
	globals::posHandler.add(handle, { xPos, yPos });
	globals::eventManager.add(this, events::KD_W);
	globals::eventManager.add(this, events::KD_A);
	globals::eventManager.add(this, events::KD_S);
	globals::eventManager.add(this, events::KD_D);

}

void Character::move(double x, double y) {
	globals::posHandler.ChangePos(handle, { x, y });
}

void Character::OnNotify(std::vector<SDL_Event*> evts)
{
	
	Pos newPosVec = { 0.0, 0.0 };
	for (SDL_Event* evt : evts) {
		events myevent = globals::eventManager.SDLtoEvent(*evt);
		switch (myevent) {
			case events::KD_W: newPosVec.yPos += 1.0;
				break;
			case events::KD_A: newPosVec.xPos += -1.0;
				break;
			case events::KD_S: newPosVec.yPos += -1.0;
				break;
			case events::KD_D: newPosVec.xPos += 1.0;
				break;
		}
	}
	if (newPosVec.xPos != 0.0 || newPosVec.yPos != 0.0) {
		double characterSpeed = 1.0;
		double distance = characterSpeed * globals::globalTimer.lastFrameTime() / 1000.0;
		double normalizer = 1 / std::sqrt(std::abs(newPosVec.xPos) + std::abs(newPosVec.yPos));
		newPosVec.xPos = newPosVec.xPos * normalizer;
		newPosVec.yPos = newPosVec.yPos * normalizer;
		move(newPosVec.xPos * distance, newPosVec.yPos * distance);
	}
	

}
