#pragma once
// game characters: have at least a quad and a position

#include "entity.h"
#include "QuadHandler.h"

class Character final: public entity {
public:
		// (image file path, quad identification name, Shader, width, height,
		// xPos and yPos are world position of entity
		// Position and Quad handlers for components of entity
		// rows, columns (of image if spritesheet), row, column of image in sheet)
		// path for image and shader files should be relative to /data/ folder
		// width and height are how much screen real estate the quad takes
	Character(QuadParams, double xPos, double yPos);

	// the amount in the x and y (horizontal and vertical respectively) directions to move the entity
	void move(double x, double y);

	// save events received into a set then evaluate 
	void OnNotify(std::vector<SDL_Event*>) override;
};