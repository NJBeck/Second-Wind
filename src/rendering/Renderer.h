#pragma once
#include "SDL2/SDL.h"
#include "glad/glad.h"

#include "handlers/PositionHandler.h"
#include "handlers/AnimationHandler.h"
#include "handlers/EventHandler.h"
#include "handlers/ShaderHandler.hpp"

#define LATEST_SDL_ERROR std::cout << "SDL failure:" << \
							SDL_GetError() << std::endl;


class Renderer
{
public:
	struct OrthoCam: public PositionHandler::Quad {
		// PositionHandler::Quad quad; // world space info of camera
		int w_width;	// camera width in pixels
		int w_height;
	};
	Renderer(SDL_Window*, PositionHandler*, QuadHandler*, AnimationHandler*,
			 ShaderHandler*);
	~Renderer();
	// draws entities located within the given camera's range
	void DrawScene();
	void AddCamera(OrthoCam const&);
	bool alive;
private:
	SDL_Window* window;
	PositionHandler* pos_handler_;
	QuadHandler* quad_handler_;
	AnimationHandler* anim_handler_;
	ShaderHandler* shader_handler_;
	std::vector<OrthoCam> cameras_; // 0th item is active cam
};

