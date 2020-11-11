#pragma once
// game characters: have at least a quad and a position

#include "entity.h"
#include "QuadHandler.h"
#include "PositionHandler.h"

// to hold the different handlers the entity might have
struct CharacterHandlers {
	PositionHandler* posHandler;
	QuadHandler* qHandler;
};

class Character: public entity {
	CharacterHandlers handlers;
public:
		// (image file path, quad identification name, Shader, width, height,
		// xPos and yPos are world position of entity
		// Position and Quad handlers for components of entity
		// rows, columns (of image if spritesheet), row, column of image in sheet)
		// path for image and shader files should be relative to /data/ folder
		// width and height are how much screen real estate the quad takes
	Character(QuadParams, double xPos, double yPos, CharacterHandlers);

	// the amount in the x and y (horizontal and vertical respectively) directions to move the entity
	void move(double x, double y);
};