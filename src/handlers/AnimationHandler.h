#pragma once
// decides when to select which quad to be actively drawn for an entity

#include "QuadHandler.h"

#include <unordered_set>
#include <cstdint>
#include <vector>

// enumerates the animations in the game
enum class AnimType {
	PLAYERWALKUP,
	PLAYERWALKRIGHT,
	PLAYERWALKLEFT,
	PLAYERWALKDOWN
};

// contains the info for the animations and state of an entity
struct AnimInfo {
	std::unordered_set<AnimType> anims;	// animations this entity has
	AnimType active_anim;				// the active animation
	uint32_t active_quad;				// index of active quad of in the cycle
	// animation implementation defined state values
	double previous;					// the previous state of the animation
	double state;						// the progress through the animation
};

class AnimationHandler {
	// holds the animation data for each entity
	std::unordered_map<EntityHandler::ID, AnimInfo> anim_data_;
	// figures which quad should be active given the current quad and animation
	// period determines the spatial length of a full cycle
	// new_pos is the distance traveled since last update
	void WalkAnim(EntityHandler::ID const, QuadParams&, double const period, 
				  double const new_pos);
public:
	// updates the entity states and sets the active quads for the entities
	// different AnimTypes update in different ways
	void Update(EntityHandler::ID const);

	// sets the active animation for the entity
	void SetActive(EntityHandler::ID const, AnimType);

	// register handle with array of animations with the first being the active
	void Add(EntityHandler::ID const, std::vector<AnimType> const);
	void Remove(EntityHandler::ID const, std::vector<AnimType> const);

	AnimationHandler(QuadHandler*, PositionHandler*);

	QuadHandler* quad_handler_;
	PositionHandler* pos_handler_;
};

