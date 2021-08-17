#include "EventManager.h"
using namespace std;

void EventHandler::Add(EntityHandler::ID const handle, Events evt)
{
	subscriptions_[evt].emplace(handle);
}

void EventHandler::Remove(EntityHandler::ID const handle, Events evt)
{
	subscriptions_[evt].erase(handle);
}

unordered_map<EntityHandler::ID, vector<EventHandler::Events>> const& 
EventHandler::GetEvents() const
{
	return notifications_;
}


void EventHandler::PollEvents()
{
	queue_.clear();
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		Events my_event = SDLtoEvent(e);
		queue_.emplace_back(my_event);
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
