#pragma once
// decides when to select which quad to be actively drawn for an entity

// TODO: make AnimInfo more general than for just quads
#include "QuadHandler.h"
#include "PositionHandler.h"

#include <unordered_set>
#include <cstdint>
#include <vector>


class AnimationHandler {
	public:
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
		u32 active_quad;				// index of active quad of in the cycle
		// animation implementation defined state values
		double previous;					// the previous state of the animation
		double state;						// the progress through the animation
	};
	// updates the entity states and sets the active quads for the entities
	// different AnimTypes update in different ways
	void Update(EntityID const);

	// sets the active animation for the entity
	void SetActive(EntityID const, AnimType);

	// register handle with array of animations with the first being the active
	void Add(EntityID const, std::vector<AnimType> const);
	void Remove(EntityID const, std::vector<AnimType> const);

	AnimationHandler(	QuadHandler& qh, 
						PositionHandler& ph) 
						: quad_handler_(qh), pos_handler_(ph) {}

private:
	// maps an animation to its quad parameters
	static const std::unordered_map<AnimType, 
							 std::vector<QuadHandler::QuadParams> >anim_quads_;
	// holds the animation data for each entity
	std::unordered_map<EntityID, AnimInfo> anim_data_;
	// figures which quad should be active given the current quad and animation
	// period determines the spatial length of a full cycle
	// new_pos is the distance traveled since last update
	void WalkAnim(EntityID const, QuadHandler::QuadParams&, 
			      double const period, double const new_pos);

	QuadHandler& quad_handler_;
	PositionHandler& pos_handler_;
};

