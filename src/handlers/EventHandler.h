#pragma once

#include "SDL2/SDL.h"
#include <deque>
#include <unordered_map>
#include <utility>
#include <unordered_set>

#include "globals.h"

class EventHandler {
public:
	enum class Event {
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
	typedef std::deque<Event> EventQueue;
	typedef std::unordered_set<Event> EventSet;
	// subscribes given entity to given events
	void Add(EntityID const, EventSet const);
	void Remove(EntityID const, EventSet const);
	// populates the notifications
	void PollEvents();
	// clears events for all entities
	void ClearEvents();
	// clears the notifications for the given entity
	void ClearNotifications(EntityID const);
	// returns events this entity is sub'd to that have occurred since last checking
	EventQueue const Notifications(EntityID const) const;

private:
	std::unordered_map<EntityID, EventSet> index_;
	// the map of entities which are subscribed to an event
	std::unordered_map<Event,
					   std::unordered_set<EntityID> > subscriptions_;
	// maps entities to their notifications
	std::unordered_map<	EntityID, 
						EventQueue > notifications_;
	// converts SDL event to our enum
	Event SDLtoEvent(SDL_Event const) const;

};