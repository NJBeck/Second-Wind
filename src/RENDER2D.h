#pragma once
#include "SDL2/SDL.h"
#include "glad/glad.h"

#include "QuadHandler.h"
#include "ImageHandler.h"
#include "Character.h"
#include "PositionHandler.h"

#define LATEST_SDL_ERROR std::cout << "SDL failure:" << SDL_GetError() << std::endl;

struct OrthoCam {
	double left;	// world position of left side of camera
	double bottom;
	double width;	// width of orthographic camera in world space
	double height;
	int w_width;	// camera width in pixels
	int w_height;
};

class RENDER2D
{
	OrthoCam cam;
	SDL_Window* window;
	QuadHandler* qHandler;
	PositionHandler* posHandler;
public:
	RENDER2D(SDL_Window*, OrthoCam, QuadHandler*, PositionHandler*);
	void DrawScene();
	~RENDER2D();

};

