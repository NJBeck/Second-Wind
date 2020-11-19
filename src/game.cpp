
#include "SDL2/SDL.h"
#include "glad/glad.h"
#include <string>

#include "RENDER2D.h"
#include "globals.h"
#include "Character.h"

int main(int argc, char* args[]) {


    // initializing SDL, GLAD and make window and context
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		LATEST_SDL_ERROR
	}
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // create camera, window and renderer
    SDL_Window* window = NULL;
    OrthoCam camera{100.0, 100.0, 16.0, 9.0, 1280, 720};
    RENDER2D renderer(window, camera);

    // create character with this quad
    QuadParams quad{ std::string("png/wulax/walkcycle/BODY_skeleton.png"), Shader("quadshader.vs", "quadshader.fs"),
                    (2 / camera.width), (2 / camera.height), 1, 4, 1, 9 };
    Character player(quad, 100.0, 100.0);

    while (renderer.alive) {
        globals::globalTimer.start();

        globals::eventManager.PollEvents();
        globals::eventManager.DispatchEvents();
        globals::movementHandler.update();

        renderer.DrawScene();

        SDL_Delay(std::max((16.666 - globals::globalTimer.elapsedMs()), 0.0));
 }
	
	SDL_Quit();

	return 0;
}