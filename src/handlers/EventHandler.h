#pragma once

#include "SDL2/SDL.h"
#include <deque>
#include <unordered_map>
#include <utility>
#include <unordered_set>
#include <optional>

#include "utility.h"
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
	KU_ESC
};
	EventHandler();
	typedef std::unordered_set<Event> EventSet;
	// subscribes given entity to given events in current queue frame
	void Subscribe(EntityID const, EventSet const&);
	// removes the entity from the subscibed events in the active frame
	void Remove(EntityID const, EventSet const);
	//// pushes new frame of suscriptions into the subscriptions queue
	//void PushSubscriptionFrame();
	//void PopSubscriptionFrame();
	// populates the polled_events_
	void PollEvents();
	void ClearEvents();
	struct Notification {
		Event event;
		std::unordered_set<EntityID> entities;
	};
	typedef std::vector<Notification> NotificationFrame;
	typedef std::deque<NotificationFrame> Notifications;
	// returns the subset of subscriptions_ that matches the polled events
	Notifications GetNotifications();
private:
	std::vector<Event> polled_events_;
	// the map of entities which are subscribed to an event
	typedef std::unordered_map<Event, std::unordered_set<EntityID>> Subscriptions;
	typedef std::deque<Subscriptions> SubscriptionsQueue;
	SubscriptionsQueue subscriptions_;
	// converts SDL event to our enum
	std::optional<Event> SDLtoEvent(SDL_Event const) const;

};