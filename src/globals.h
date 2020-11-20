#pragma once
#include "handlers/PositionHandler.h"
#include "handlers/QuadHandler.h"
#include "handlers/ImageHandler.h"
#include "handlers/MovementHandler.h"
#include "handlers/EventManager.h"
#include "utility.h"

namespace globals {
	extern PositionHandler posHandler;
	extern ImageHandler imgHandler;
	extern QuadHandler quadHandler;
	extern EventManager eventManager;
	extern MovementHandler movementHandler;
	extern Timer globalTimer;
}