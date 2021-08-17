#pragma once
#include "SDL2/SDL.h"
#include <vector>

#include "handlers/AnimationHandler.h"
#include "handlers/PositionHandler.h"
#include "handlers/EventManager.h"
#include "handlers/MovementHandler.h"
#include "rendering/RENDER2D.h"

class EntityHandler {
public:
	typedef uint64_t ID;
	enum class Type {
		Character,	
		Renderer
	};

	EntityHandler(AnimationHandler*, Renderer*,
				  EventHandler*, MovementHandler*);
	void UpdateEvents();
	// create character with given spatial information
	void AddCharacter(PositionHandler::Quad);
	void AddRenderer();

private:
	void NotifyCharacter(std::vector<EventHandler::Events> const&, ID const);
	void NotifyRenderer(std::vector<EventHandler::Events> const&, ID const);

	static ID count;
	AnimationHandler* anim_handler_;
	QuadHandler* quad_handler_;
	PositionHandler* pos_handler_;
	EventHandler* event_handler_;
	MovementHandler* move_handler_;
	Renderer* renderer_;
	// maps an entity to its type
	std::unordered_map<ID, Type> type_map_;
};

