
#include "globals.h"
#include "entities/Character.h"


int main(int argc, char* args[]) {
    Character player(100.0, 100.0);
    while (globals::renderer.alive) {
        globals::globalTimer.start();

        globals::eventManager.PollEvents();
        globals::eventManager.DispatchEvents();
        globals::movementHandler.update();

        globals::renderer.DrawScene();
 }
	
	SDL_Quit();

	return 0;
}