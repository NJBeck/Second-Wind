#pragma once
#include "SDL2/SDL.h"
#include "glad/glad.h"

#include "entities/entity.h"

#define LATEST_SDL_ERROR std::cout << "SDL failure:" << SDL_GetError() << std::endl;

struct OrthoCam {
	double left;	// world position of left side of camera
	double bottom;
	double width;	// width of orthographic camera in world space
	double height;
	int w_width;	// camera width in pixels
	int w_height;
};

class RENDER2D final: public entity
{
	SDL_Window* window;
public:
	OrthoCam cam;
	RENDER2D(SDL_Window*, OrthoCam);
	void DrawScene();
	~RENDER2D();

	void OnNotify(std::vector<SDL_Event*>) override;
	bool alive;
};

