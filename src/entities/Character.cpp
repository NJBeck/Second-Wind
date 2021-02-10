#include "Character.h"
#include "globals.h"

using std::vector, std::string;

Character::Character(double xPos, double yPos)
{
	// generate the quads for the character
	vector<QuadParams> quads;
	string img{"walkcycle/BODY_male.png"};
	string vs{ "quadshader.vs" };
	string fs{ "quadshader.fs" };
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 4; ++j) {
			quads.push_back({	img, vs, fs, 
								2 / static_cast<float>(globals::camera.width), 
								2 / static_cast<float>(globals::camera.height), 
								j, 4, 
								i, 9 });
		}
	}
	// add components
	globals::quadHandler.add(handle, quads, 33);
	globals::animHandler.add(handle, 
							{ AnimType::PLAYERWALKUP, AnimType::PLAYERWALKDOWN, AnimType::PLAYERWALKLEFT, AnimType::PLAYERWALKRIGHT }, 
							AnimType::PLAYERWALKDOWN);
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
			globals::animHandler.SetActive(handle, AnimType::PLAYERWALKUP);
		}
			break;
		case events::KD_A: {
			inputXDir += -1;
			globals::eventManager.remove(this, events::KD_A);
			globals::eventManager.add(this, events::KU_A);
			globals::animHandler.SetActive(handle, AnimType::PLAYERWALKLEFT);
		}
			break;
		case events::KD_S: {
			inputYDir += -1;
			globals::eventManager.remove(this, events::KD_S);
			globals::eventManager.add(this, events::KU_S);
			globals::animHandler.SetActive(handle, AnimType::PLAYERWALKDOWN);
		}
			break;
		case events::KD_D: {
			inputXDir += 1;
			globals::eventManager.remove(this, events::KD_D);
			globals::eventManager.add(this, events::KU_D);
			globals::animHandler.SetActive(handle, AnimType::PLAYERWALKRIGHT);
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
