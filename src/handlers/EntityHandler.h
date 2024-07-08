#pragma once
// the type of entity is determined by the handlers it is handled by
// and the behaviors that modify their state
// events are polled and then handled by each subscribed entity
// according to their behavior
#include "SDL2/SDL.h"
#include <memory>
#include <deque>
#include <queue>
#include <vector>

#include "AnimationHandler.h"
#include "EventHandler.h"
#include "MovementHandler.h"
#include "rendering/Renderer.h"
#include "ShaderHandler.h"
#include "globals.h"
#include "utility.h"


class EntityHandler {
public:
	EntityHandler();
	
	EntityID MakePlayer( PositionHandler::Box const);
	EntityID MakeRenderer();
	EntityID MakeCamera( PositionHandler::Box const);
	void ActiveCam(	EntityID const);

	void Remove(EntityID const);
	void Update();
	bool Alive();
private:
	/*
	entity behavior stuff:
	entities are defined by attributes (ie membership in certain handlers/have state)
	and behaviors (which define the modification of the attributes)
	*/
	enum class Behavior {
		Controllable,
		Walk,
		Exit
	};

	// for each entity an event maps to a series of behaviors
	typedef std::unordered_map<	EventHandler::Event, 
								std::deque<Behavior>> EventBehaviors;
	struct EntityData {
		EventBehaviors event_behaviors;
		std::unordered_set<Behavior> behavior_set;
	};
	std::unordered_map<EntityID, EntityData> index_;
	// adds the given queue of behaviors to each event
	void AddBehavior(EntityID const, 
		std::vector<EventHandler::Event> const&, std::deque<Behavior> const&);
	void HandleEventBehavior(EntityID const, EventHandler::Event const);
	/************************************************************************************
	functions for how each behavior handles its events
	*/
	void ControllableEvent(EntityID const, EventHandler::Event const);
	void WalkEvent(EntityID const);
	void ExitEvent();
	/************************************************************************************
	entity generation stuff
	*/
	EntityID AddEntity();	// generates new handle
	void RemoveEntity(EntityID);	// retires handle

	EntityID highest_count_;
	// presents the largest used handle so we can reuse handles
	std::priority_queue<EntityID> used_handles_;

	void AddQuadShader(EntityID const);
	EntityID MakeCharacter( PositionHandler::Box const, 
		ShaderHandler::VertexShader const,
		ShaderHandler::FragmentShader const, 
		std::unordered_set<AnimationHandler::AnimType> const&);
	/************************************************************************************
	handler data members
	*/
	bool alive;
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


