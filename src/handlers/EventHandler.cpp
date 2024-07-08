#include "EventHandler.h"
#include "utility.h"
using std::vector, std::unordered_map, std::unordered_set, std::deque,
	std::optional;

EventHandler::EventHandler()
{
	subscriptions_.push_front({});
}

void EventHandler::Subscribe(EntityID const handle, EventSet const& evts)
{
	auto& top_subs = subscriptions_[0];
	for (auto& evt : evts) {
		auto found_evt = top_subs.find(evt);
		if (found_evt != top_subs.end()) {
			auto& entities = found_evt->second;
			entities.emplace(handle);
		}
		else {
			top_subs[evt] = { handle };
		}
	}
}

void EventHandler::Remove(EntityID const handle, EventSet const evts)
{
	auto& top_subs = subscriptions_.front();
	for (auto& evt : evts) {
		auto found_evt = top_subs.find(evt);
		if (found_evt != top_subs.end()) {
			auto& evt_ents = found_evt->second;
			auto found_ent = evt_ents.find(handle);
			if (found_ent != evt_ents.end()) {
				evt_ents.erase(handle);
				if (evt_ents.size() == 0) {
					top_subs.erase(evt);
				}
			}
		}
	}
}

void EventHandler::ClearEvents()
{
	polled_events_.clear();
}


EventHandler::Notifications EventHandler::GetNotifications()
{
	Notifications notifs;
	for (auto& subs_frame : subscriptions_) {
		Notification evt_ent;
		NotificationFrame event_frame;
		for (auto evt : polled_events_) {
			// for each polled event check if there is a subscriber in that frame
			auto found_event = subs_frame.find(evt);
			if (found_event != subs_frame.end()) {
				evt_ent.event = evt;
				evt_ent.entities = found_event->second;
				event_frame.push_back(evt_ent);
			}
			
		}
		notifs.push_back(event_frame);
	}
	return notifs;
}


void EventHandler::PollEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		auto my_evt = SDLtoEvent(e);
		if (my_evt.has_value()) {
			polled_events_.emplace_back(my_evt.value());
		}
	}
}

optional<EventHandler::Event> EventHandler::SDLtoEvent(SDL_Event const sdlevt) const{
	switch (sdlevt.type) {
		case SDL_KEYDOWN: {
			switch (sdlevt.key.keysym.sym) {
				case SDLK_w: return Event::KD_W;
		
				case SDLK_a: return Event::KD_A;
		
				case SDLK_s: return Event::KD_S;
		
				case SDLK_d: return Event::KD_D;
	
				case SDLK_ESCAPE: return Event::KD_ESC;
					
				default: return {};
			}
			break;
		}
		case SDL_KEYUP: {
			switch (sdlevt.key.keysym.sym) {
			case SDLK_w: return Event::KU_W;

			case SDLK_a: return Event::KU_A;

			case SDLK_s: return Event::KU_S;

			case SDLK_d: return Event::KU_D;

			case SDLK_ESCAPE: return Event::KU_ESC;

			default: return {};
			}
			break;
		}
		case SDL_MOUSEMOTION: return Event::MM;

		case SDL_MOUSEBUTTONDOWN: return Event::MBD;

		default: return {};
	}
}
