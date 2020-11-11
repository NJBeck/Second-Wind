// TODO:    add event handler, movement/transform components
//          possibly make single component handler that has all the information
//              for which handlers an entity has so the entity only knows about this one handler

#include "SDL2/SDL.h"
#include "glad/glad.h"

#include "RENDER2D.h"

using std::string;

int main(int argc, char* args[]) {


    // initializing SDL, GLAD and make window and context
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		LATEST_SDL_ERROR
	}
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // create camera, entity component managers and renderer
    SDL_Window* window = NULL;
    OrthoCam camera{100.0, 100.0, 16.0, 9.0, 1280, 720};
    ImageHandler imageHandler;
    QuadHandler quadHandler(&imageHandler);
    PositionHandler posHandler;
    RENDER2D renderer(window, camera, &quadHandler, &posHandler);

    // create character with this quad
    QuadParams quad{ string("png/wulax/walkcycle/BODY_male.png"), Shader("quadshader.vs", "quadshader.fs"),
                    0.4, 0.4, 1, 4, 1, 9 };
    CharacterHandlers charHandlers { &posHandler, &quadHandler };
    Character player(quad, 100.1, 100.1, charHandlers);

    while (true) {

        auto startTime = SDL_GetPerformanceCounter();
        auto perfFreq = SDL_GetPerformanceFrequency();

        renderer.DrawScene();

        float elapsedMS = ((float)(SDL_GetPerformanceCounter() - startTime)) / (float)perfFreq * 1000;


        SDL_Delay(std::max((16.666 - elapsedMS), 0.0));
    }
	

	SDL_Quit();

	return 0;
}