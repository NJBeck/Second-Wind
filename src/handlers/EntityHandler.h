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
#include "utility.h"

class EntityHandler {
public:
	EntityHandler();

	void UpdateEvents();
	// we would like the Add function to guide us with the minimum inputs
	// to create an instance of the given EntityType
	template<typename T>
	EntityID NewEntity(T const& type_args);
	
	// gives the attributes and behaviors of T to entity
	template<typename T>
	EntityID AddType(EntityID const handle, T const& type_args);

	void Remove(EntityID);

	//
	// Entity Type declarations
	//
	struct Entity {
		Entity();
	};

	struct Character : Entity{
		struct InitParams {
			PositionHandler::Box  position_params;
			MovementHandler::Velocity  velocity;
			std::vector<QuadHandler::QuadParams>  quad_params;
			ShaderHandler::VertexShader  vertex_shader;
			ShaderHandler::FragmentShader  fragment_shader;
		};
		Character(InitParams const&);
	};

	struct Player : Character{
		Player(Character::InitParams const&);
	};


private:

	/*
	entity behavior stuff
		entities are defined by attributes (ie membership in certain handlers)
		and behaviors (which define what and when it is added to a handler)
	*/
	enum class Behavior {
		Controllable,
		Walk
	};
	// Events are handled by behaviors in a priority ordering
	typedef UniqueDeque<Behavior> BehaviorPriority;
	typedef std::unordered_map<	EventHandler::Event, 
								BehaviorPriority> EventBehaviorPriority;
	struct EntityBehaviors {
		EntityBehaviors(EventBehaviorPriority const& evt_pr,
						std::unordered_set<Behavior> const& ent_b) :
						event_priorities(evt_pr), entity_behaviors(ent_b) {};
		EventBehaviorPriority event_priorities;
		std::unordered_set<Behavior> entity_behaviors;
	};
	// maps an entity to its behaviors and their priorities of execution
	std::unordered_map<EntityID, EntityBehaviors> entity_behaviors_;
	// adds the behaviors to the event in the EventBehaviorPriority map in vector order
	void MakeEventPriority(	EventBehaviorPriority&, EventHandler::Event, 
							std::vector<Behavior> const&);
	// adds the given behavior to given EventBehaviorPriority event with the given order
	// for each event
	void AddBehaviorPriority(	EventBehaviorPriority&, 
								Behavior, 
								std::unordered_map<EventHandler::Event, u32> const&);
	// adds the behavior to the given entity
	void AddBehavior(EntityID , Behavior);

	/*
	functions for how each behavior handles its events
	*/
	void BehaviorEvent(	EntityID const& handle,
						EventHandler::Event const& event,
						Behavior const& behavior);
	void ControllableEvent(EntityID const& handle, EventHandler::Event const& event);

	/*
	entity generation stuff
	*/
	EntityID AddEntity();
	void RemoveEntity(EntityID);
	EntityID highest_count_;
	// presents the largest used handle so we can reuse handles
	std::priority_queue<EntityID> used_handles_;

	/*
	handler data members
	*/
	std::unique_ptr<ImageHandler> img_handler_;
	std::unique_ptr<QuadHandler> quad_handler_;
	std::unique_ptr<AnimationHandler> anim_handler_;
	std::unique_ptr<EventHandler> event_handler_;
	std::unique_ptr<MovementHandler> move_handler_;
	std::unique_ptr<PositionHandler> position_handler_;
	std::unique_ptr<Renderer> renderer_;
	std::unique_ptr<ShaderHandler> shader_handler_;
	std::unique_ptr<Timer> timer_;
};


