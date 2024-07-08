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
	// register handle with array of animations
	void Add(EntityID const, std::unordered_set<AnimType> const);
	void Remove(EntityID const, std::unordered_set<AnimType> const);

	void ToUpdate(EntityID const);
	void Update();

	// sets the active animation for the entity
	void SetActive(EntityID const, AnimType);


	AnimationHandler(	QuadHandler& qh, 
						PositionHandler& ph) 
						: quad_handler_(qh), pos_handler_(ph) {}

private:
	std::unordered_set<EntityID> to_update_;	// the entities to be updated
	// maps an animation to its quad parameters
	static const std::unordered_map<AnimType, 
							 std::vector<QuadHandler::QuadParams> >anim_quads_;
	// contains the info for the animations and state of an entity
	struct AnimInfo {
		std::unordered_set<AnimType> anims;	// animations this entity has
		AnimType active_anim;				// the active animation
		// animation implementation defined state values
		double previous;					// the previous state of the animation
		double state;						// the progress through the animation
	};
	std::unordered_map<EntityID, AnimInfo> index_;
	// figures which quad should be active given the current quad and animation
	// period determines the spatial length of a full cycle
	// new_pos is the distance traveled since last update
	void WalkAnim(EntityID const, QuadHandler::QuadParams&, 
			      double const period, double const new_pos);
	QuadHandler& quad_handler_;
	PositionHandler& pos_handler_;
};

