#pragma once
// TODO: add EntityManager to handle entities and their events (should take care of this nasty pointer business)

#include "SDL2/SDL.h"
#include <vector>
#include <unordered_map>
#include <utility>
#include <unordered_set>

#include "handlers/EntityHandler.h"


class EventHandler {
public:
	enum class Events {
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
	void PollEvents();				// makes the vector of events queue_
	void Add(EntityHandler::ID const, Events);	// subscribe entity to event
	void Remove(EntityHandler::ID const, Events);	// unsubscribe entity
	std::unordered_map<EntityHandler::ID,
					   std::vector<Events> > const& GetEvents() const;

private:
	Events SDLtoEvent(SDL_Event const) const;
	// the map of entities which are subscribed to an event
	std::unordered_map<Events, 
					   std::unordered_set<EntityHandler::ID> > subscriptions_;
	// maps entities to their notifications
	std::unordered_map<EntityHandler::ID, 
					   std::vector<Events> > notifications_;
	// the events get dispatched from a queue
	std::vector<Events> queue_;
};