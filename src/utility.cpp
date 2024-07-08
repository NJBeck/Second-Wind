
#include <iostream>

#include "utility.h"

using std::filesystem::path, std::string;

#define LATEST_SDL_ERROR std::cout << "SDL failure:" << \
                         SDL_GetError() << std::endl;

namespace utility {

    string getDataPath(string extensionString) {

        path extensionPath(extensionString, path::native_format);
        path dataPath;
        // current path remains the working directory
        auto currentPath = std::filesystem::current_path();
        // go up the path until we find "second wind"
        auto currentDir = currentPath.end();
        --currentDir;
        while (*currentDir != "Second-Wind") {
            --currentDir;
        }
        ++currentDir;
        // currentDir should now point to the "second wind" root folder
        for (auto it = currentPath.begin(); it != currentDir; ++it) {
            dataPath /= *it;
        }
        dataPath /= "data";
        dataPath /= extensionPath;
        return dataPath.string();
    }

    void InitGL(SDL_Window** wind, int width, int height) {
        SDL_Window* window = *wind;
        // initializing SDL, GLAD and make window and context
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            LATEST_SDL_ERROR
                SDL_Quit();
        }
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        window = SDL_CreateWindow("Second Wind", SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
        if (window == NULL)
        {
            LATEST_SDL_ERROR
        }
        SDL_GLContext GLcontext = SDL_GL_CreateContext(window);
        if (!gladLoadGL()) {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }
        SDL_GL_SetSwapInterval(1);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        *wind = window;

    }

    GLenum glCheckError_(const char* file, int line)
    {
        GLenum errorCode;
        while ((errorCode = glGetError()) != GL_NO_ERROR)
        {
            std::string error;
            switch (errorCode)
            {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            std::cout << error << " | " << file << " (" << line << ")" << std::endl;
        }
        return errorCode;
    }

}

Timer::Timer() : perfFreq(SDL_GetPerformanceFrequency()), startTime(SDL_GetPerformanceCounter()), lastTime(0.0) {}

float Timer::elapsedMs() {
    return  ((float)(SDL_GetPerformanceCounter() - startTime)) / (float)perfFreq * 1000;
}

void Timer::start() {
    lastTime = elapsedMs();
    startTime = SDL_GetPerformanceCounter();
}

float Timer::lastFrameTime() {
    return lastTime;
}