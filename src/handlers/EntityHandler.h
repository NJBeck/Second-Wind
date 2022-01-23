#pragma once
#include "SDL2/SDL.h"
#include <vector>

#include "AnimationHandler.h"
#include "EventHandler.h"
#include "MovementHandler.h"
#include "rendering/Renderer.h"
#include "ShaderHandler.hpp"
#include "globals.h"

class EntityHandler {
public:
	// the different types of entities
	enum class EntityType {
		Character,	
		Renderer,
		OrthoCamera
	};

	EntityHandler(AnimationHandler*, Renderer*,
				  EventHandler*, MovementHandler*, 
				  ShaderHandler*, PositionHandler*);
	void UpdateEvents();
	// create character with given spatial information
	EntityID AddCharacter(PositionHandler::Quad);
	EntityID AddOrthoCamera(PositionHandler::Quad);

private:
	EntityID AddEntity(EntityType const);
	EntityID AddRenderer();
	// functions to handle how each entity type handles events
	void NotifyCharacter(std::vector<EventHandler::Events> const&, 
						 EntityID const);
	void NotifyRenderer(std::vector<EventHandler::Events> const&, 
						EntityID const);
	//data members
	static EntityID count;
	AnimationHandler* anim_handler_;
	EventHandler* event_handler_;
	MovementHandler* move_handler_;
	PositionHandler* position_handler_;
	Renderer* renderer_;
	ShaderHandler* shader_handler_;
	// maps an entity to its type
	std::unordered_map<EntityID, EntityType> type_map_;
};

