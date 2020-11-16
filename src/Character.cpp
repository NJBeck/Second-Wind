#include "Character.h"
#include "globals.h"

Character::Character(QuadParams quad, double xPos, double yPos)
{
	// add the position and quad components
	globals::quadHandler.add(handle, quad);
	globals::posHandler.add(handle, { xPos, yPos });
	globals::eventManager.add(this, events::KD_W);
	globals::eventManager.add(this, events::KD_A);
	globals::eventManager.add(this, events::KD_S);
	globals::eventManager.add(this, events::KD_D);
	globals::movementHandler.add(handle, 1.0);
}

void Character::OnNotify(std::vector<SDL_Event*> evts)
{
	int inputXDir = 0;
	int inputYDir = 0;
	for (SDL_Event* evt : evts) {
		events myevent = globals::eventManager.SDLtoEvent(*evt);
		switch (myevent) {
		case events::KD_W: {
			inputYDir += 1;
			globals::eventManager.remove(this, events::KD_W);
			globals::eventManager.add(this, events::KU_W);
		}
			break;
		case events::KD_A: {
			inputXDir += -1;
			globals::eventManager.remove(this, events::KD_A);
			globals::eventManager.add(this, events::KU_A);
		}
			break;
		case events::KD_S: {
			inputYDir += -1;
			globals::eventManager.remove(this, events::KD_S);
			globals::eventManager.add(this, events::KU_S);
		}
			break;
		case events::KD_D: {
			inputXDir += 1;
			globals::eventManager.remove(this, events::KD_D);
			globals::eventManager.add(this, events::KU_D);
		}
			break;
		case events::KU_W: {
			inputYDir += -1;
			globals::eventManager.remove(this, events::KU_W);
			globals::eventManager.add(this, events::KD_W);
		}
			break;
		case events::KU_A: {
			inputXDir += 1;
			globals::eventManager.remove(this, events::KU_A);
			globals::eventManager.add(this, events::KD_A);
		}
						 break;
		case events::KU_S: {
			inputYDir += 1;
			globals::eventManager.remove(this, events::KU_S);
			globals::eventManager.add(this, events::KD_S);
		}
						 break;
		case events::KU_D: {
			inputXDir += -1;
			globals::eventManager.remove(this, events::KU_D);
			globals::eventManager.add(this, events::KD_D);
		}
						 break;
		}
	}
	globals::movementHandler.addVelocity(handle, (double)inputXDir, (double)inputYDir);

}
