#include "SDL2/SDL.h"
#include <iostream>

#include "utility.h"

using std::filesystem::path, std::string;

namespace utility {

    string getDataPath(string extensionString) {

        path extensionPath(extensionString, path::native_format);
        path dataPath;
        // current path remains the working directory
        auto currentPath = std::filesystem::current_path();
        // go up the path until we find "second wind"
        auto currentDir = currentPath.end();
        --currentDir;
        while (*currentDir != "second wind") {
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