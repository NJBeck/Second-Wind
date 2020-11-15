#include "globals.h"

namespace globals {
	PositionHandler posHandler;
	ImageHandler imgHandler;
	QuadHandler quadHandler(&imgHandler);
	EventManager eventManager;
	Timer globalTimer;

}