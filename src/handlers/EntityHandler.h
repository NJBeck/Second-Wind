#pragma once
// TODO: split the updating of events off into a game logic handler
#include "SDL2/SDL.h"
#include <memory>
#include <queue>
#include <vector>

#include "AnimationHandler.h"
#include "EventHandler.h"
#include "MovementHandler.h"
#include "rendering/Renderer.h"
#include "ShaderHandler.hpp"
#include "globals.h"

class EntityHandler {
public:
	// entities are defined by attributes (ie registered to certain handlers) and behaviors
	enum class Attributes {
		Position,
		Shader,
		Movement,
		Image,
		Quad,
		Event,
		Animation
	};

	enum class Behaviors {
		Movement
	};

	EntityHandler();
	void UpdateEvents();
	// we would like the Add function to guide us with the minimum inputs
	// to create an instance of the given EntityType
	template<typename T, typename... Args>
	EntityID Add(Args&&... args);

	void Remove(EntityID);

	// Entity Type declarations
	struct Entity {
		Entity();
		EntityID ID;
	};

	struct Character : Entity{
		Character(PositionHandler::Box const&);
	};
private:
	static EntityID AddEntity();
	void RemoveEntity(EntityID);

	// functions to handle how each entity type handles events
	void Notify(EntityID, EventHandler::EventQueue const&);

	template<typename T>
	void NotifyEntity(EventHandler::EventQueue const&);

	//data members
	static std::unique_ptr<AnimationHandler> anim_handler_;
	static std::unique_ptr<EventHandler> event_handler_;
	static std::unique_ptr<MovementHandler> move_handler_;
	static std::unique_ptr<PositionHandler> position_handler_;
	static std::unique_ptr<Renderer> renderer_;
	static std::unique_ptr<ShaderHandler> shader_handler_;
	static std::unique_ptr<Timer> timer_;
	// maps an entity to its type
	static std::unordered_map<EntityID, EntityType> index_;
	static EntityID highest_count_;
	// presents the largest used handle so we can reuse handles
	static std::priority_queue<EntityID> used_handles_;
};


