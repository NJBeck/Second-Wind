#pragma once

#include "SDL2/SDL.h"
#include <vector>
#include <unordered_map>
#include <utility>
#include <set>

#include "entities/entity.h"

enum class events {
	KD_A, // keydown events
	KD_B,
	KD_C,
	KD_D,
	KD_E,
	KD_F,
	KD_G,
	KD_H,
	KD_I,
	KD_J,
	KD_K,
	KD_L,
	KD_M,
	KD_N,
	KD_O,
	KD_P,
	KD_Q,
	KD_R,
	KD_S,
	KD_T,
	KD_U,
	KD_V,
	KD_W,
	KD_X,
	KD_Y,
	KD_Z,
	KD_ESC,
	MM,		// mouse motion event
	MBD,	// mouse button down
	MBU,	// mouse button up
	KU_A,	// key up events
	KU_B,
	KU_C,
	KU_D,
	KU_E,
	KU_F,
	KU_G,
	KU_H,
	KU_I,
	KU_J,
	KU_K,
	KU_L,
	KU_M,
	KU_N,
	KU_O,
	KU_P,
	KU_Q,
	KU_R,
	KU_S,
	KU_T,
	KU_U,
	KU_V,
	KU_W,
	KU_X,
	KU_Y,
	KU_Z,
	KU_ESC,
	QUIT
};

class EventManager {
	// the map of entities which are subscribed to an event
	std::unordered_map<events, std::set<entity*>> eventMap;
	// maps entities to the indices of the events they were subscribed to in eventQueue
	std::unordered_map<entity*, std::vector<SDL_Event*>> notifications;

	// the events get dispatched from a queue
	std::vector<SDL_Event> eventQueue;
public:
	void PollEvents();				// makes the vector of events eventQueue
	void DispatchEvents();			// gives each entity their notifications
	void add(entity*, events);		// subscribe entity to event
	void remove(entity*, events);	// unsubscribe entity
	events SDLtoEvent(SDL_Event);	// translates SDL_Event to my event

};