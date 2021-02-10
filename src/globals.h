#pragma once
#include "handlers/PositionHandler.h"
#include "handlers/QuadHandler.h"
#include "handlers/ImageHandler.h"
#include "handlers/MovementHandler.h"
#include "handlers/EventManager.h"
#include "rendering/RENDER2D.h"
#include "handlers/AnimationHandler.h"
#include "utility.h"

namespace globals {
	extern RENDER2D renderer;
	extern PositionHandler posHandler;
	extern ImageHandler imgHandler;
	extern QuadHandler quadHandler;
	extern EventManager eventManager;
	extern MovementHandler movementHandler;
	extern Timer globalTimer;
	extern AnimationHandler animHandler;
	extern OrthoCam camera;
	extern SDL_Window* window;
}