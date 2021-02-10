#include "globals.h"
// TODO: change constructors to take the required handlers 
//		 so order of initialization is clear
//		 e.g. quadHandler -> quadHandler(&imgHandler)

namespace globals {
	SDL_Window* window = NULL;
	OrthoCam camera{ 100.0, 100.0, 16.0, 9.0, 1280, 720 };
	EventManager eventManager;
	// renderer inits SDL so nothing that calls SDL should be initialized before it
	RENDER2D renderer(window, camera);

	ImageHandler imgHandler;
	QuadHandler quadHandler;
	AnimationHandler animHandler;
	PositionHandler posHandler;
	MovementHandler movementHandler;
	Timer globalTimer;

}