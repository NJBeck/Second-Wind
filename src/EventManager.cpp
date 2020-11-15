#include "EventManager.h"


void EventManager::add(entity* e, events evt)
{
	eventMap[evt].emplace(e);
}

void EventManager::remove(entity* e, events evt)
{
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
						break;
				case SDLK_a: return events::KD_A;
						break;
				case SDLK_s: return events::KD_S;
						break;
				case SDLK_d: return events::KD_D;
					break;
				case SDLK_ESCAPE: return events::KD_ESC;
					break;
			}
			break;
		}
		case SDL_MOUSEMOTION: return events::MM;
			break;
		case SDL_MOUSEBUTTONDOWN: return events::MBD;
			break;
	}
}
