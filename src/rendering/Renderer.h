#pragma once
#include "SDL2/SDL.h"
#include "glad/glad.h"

#include "handlers/PositionHandler.h"
#include "handlers/QuadHandler.h"
#include "handlers/EventManager.h"

#define LATEST_SDL_ERROR std::cout << "SDL failure:" << SDL_GetError() << std::endl;

struct OrthoCam {
	PositionHandler::Quad quad; // world space info of camera
	int w_width;	// camera width in pixels
	int w_height;
};

class Renderer
{
	SDL_Window* window;
	PositionHandler* pos_handler_;
	QuadHandler* quad_handler_;
	AnimationHandler* anim_handler_;
	OrthoCam camera;
public:
	bool alive;
	Renderer(SDL_Window*, PositionHandler*, QuadHandler*, AnimationHandler*);
	~Renderer();
	void DrawScene();

};

