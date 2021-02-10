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
	AnimType activeAnim;				// the active animation
	unsigned activeQuad;				// index of active quad of in the cycle

	// animation implementation defined state values
	double previous;					// the previous state of the animation
	double state;						// the progress through the active animation
};

class AnimationHandler {
	// holds the animation data for each entity
	std::unordered_map<uint64_t, AnimInfo> animData;
	void _WalkAnim(uint64_t, QuadParams&, double const, double const);
public:
	// updates the entity states and sets the active quads for the entities
	// different AnimTypes update in different ways
	void update(uint64_t);

	// sets the active animation for the entity
	void SetActive(uint64_t, AnimType);

	// register handle with array of animations, and active animation
	void add(uint64_t, std::vector<AnimType>, AnimType);
	void del(uint64_t);

};

