#include "EventManager.h"


void EventManager::add(entity* e, events evt)
{
	eventMap[evt].emplace(e);
}

void EventManager::remove(entity* e, events evt)
{
	eventMap[evt].erase(e);
}


void EventManager::PollEvents()
{
	eventQueue.clear();
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		eventQueue.push_back(e);
		// if entities are subscribed to this event we add it to their notifications
		events myEvent = SDLtoEvent(e);
		for (entity* ent : eventMap[myEvent]) {
			notifications[ent].push_back(&eventQueue.back());
		}

	}
}

void EventManager::DispatchEvents()
{
	for (auto& ent : notifications) {
		ent.first->OnNotify(ent.second);
	}
	notifications.clear();
}

events EventManager::SDLtoEvent(SDL_Event sdlevt) {
	switch (sdlevt.type) {
		case SDL_KEYDOWN: {
			switch (sdlevt.key.keysym.sym) {
				case SDLK_w: return events::KD_W;
		
				case SDLK_a: return events::KD_A;
		
				case SDLK_s: return events::KD_S;
		
				case SDLK_d: return events::KD_D;
	
				case SDLK_ESCAPE: return events::KD_ESC;
			}
			break;
		}
		case SDL_KEYUP: {
			switch (sdlevt.key.keysym.sym) {
			case SDLK_w: return events::KU_W;

			case SDLK_a: return events::KU_A;

			case SDLK_s: return events::KU_S;

			case SDLK_d: return events::KU_D;

			case SDLK_ESCAPE: return events::KU_ESC;
			}
			break;
		}
		case SDL_MOUSEMOTION: return events::MM;

		case SDL_MOUSEBUTTONDOWN: return events::MBD;

		case SDL_QUIT: return events::QUIT;
	}
}
