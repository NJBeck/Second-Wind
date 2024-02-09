#include "EventHandler.h"
#include "utility.h"
using std::vector, std::unordered_map;

void EventHandler::Add(EntityID const handle, EventSet const evts)
{
	auto& ent_evts = index_[handle];
	for (auto& evt : evts) {
		subscriptions_[evt].emplace(handle);
		ent_evts.insert(evt);
	}
}

void EventHandler::Remove(EntityID const handle, EventSet const evts)
{
	if (!utility::InUMap(handle, index_)) {
		utility::Log("EventHandler", "removing Event from", "unregistered entity");
		return;
	}
	auto& evt_set = index_[handle];
	for (auto& evt : evts) {
		// check if entity has this event
		if (!utility::InUSet(evt, evt_set)) {
			utility::Log("EventHandler", "removing unregistered Event from", "Entity");
		}
		else {
			evt_set.erase(evt);
			// if entity has no more subscriptions then erase it
			if (evt_set.size() == 0) {
				notifications_.erase(handle);
				return;
			}
			// check if event even has subscriptions
			if (!utility::InUMap(evt, subscriptions_)) {
				utility::Log("EventHandler", "removing Event", "without subscriptions");
			}
			else {
				// finally check that entity has this subscription
				auto& entity_set = subscriptions_[evt];
				if (!utility::InUSet(handle, entity_set)) {
					utility::Log(	"EventHandler", 
									"removing unsubscribed Entity", 
									"from subscriptions");
				}
				else {
					entity_set.erase(handle);
					// if no more subscribers then erase it
					if (entity_set.size() == 0) subscriptions_.erase(evt);
				}
			}
		}
	}
}

EventHandler::EventQueue const EventHandler::Notifications(EntityID const handle) const {
	EventQueue evt_q;
	auto found = notifications_.find(handle);
	if (found != notifications_.end()) {
		evt_q = found->second;
	}
	return evt_q;
}

void EventHandler::ClearEvents()
{
	notifications_.clear();
}

void EventHandler::ClearNotifications(EntityID const handle) {
	if (notifications_.find(handle) != notifications_.end()) {
		notifications_[handle].clear();
	}
}

void EventHandler::PollEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		Events my_event = SDLtoEvent(e);
		// if entities are subscribed to this event we add to notifications
		auto found = subscriptions_.find(my_event);
		if (found != subscriptions_.end()) {
			for (auto& handle : found->second) {
				notifications_[handle].emplace_back(my_event);
			}
		}
	}
}

EventHandler::Events EventHandler::SDLtoEvent(SDL_Event const sdlevt) const{
	switch (sdlevt.type) {
		case SDL_KEYDOWN: {
			switch (sdlevt.key.keysym.sym) {
				case SDLK_w: return Events::KD_W;
		
				case SDLK_a: return Events::KD_A;
		
				case SDLK_s: return Events::KD_S;
		
				case SDLK_d: return Events::KD_D;
	
				case SDLK_ESCAPE: return Events::KD_ESC;
			}
			break;
		}
		case SDL_KEYUP: {
			switch (sdlevt.key.keysym.sym) {
			case SDLK_w: return Events::KU_W;

			case SDLK_a: return Events::KU_A;

			case SDLK_s: return Events::KU_S;

			case SDLK_d: return Events::KU_D;

			case SDLK_ESCAPE: return Events::KU_ESC;
			}
			break;
		}
		case SDL_MOUSEMOTION: return Events::MM;

		case SDL_MOUSEBUTTONDOWN: return Events::MBD;

		case SDL_QUIT: return Events::QUIT;
	}
}
