#pragma once
#include "PositionHandler.h"
#include "QuadHandler.h"
#include "MovementHandler.h"
#include "EventManager.h"
#include "utility.h"

namespace globals {
	extern PositionHandler posHandler;
	extern ImageHandler imgHandler;
	extern QuadHandler quadHandler;
	extern EventManager eventManager;
	extern MovementHandler movementHandler;
	extern Timer globalTimer;
}