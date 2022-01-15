#pragma once

#include <filesystem>
#include <string>
#include "glad/glad.h"
#include "SDL2/SDL.h"


namespace utility {
    // gives the absolute path to the asset from the relative path in /data/ folder
    std::string getDataPath(std::string);

    void InitGL(SDL_Window** , int width, int height);

    GLenum glCheckError_(const char* file, int line);

}

class Timer {
    Uint64 startTime;
    Uint64 perfFreq;
    float lastTime;
public:
    Timer();
    void start();           // starts timer
    float elapsedMs();      // time since timer started
    float lastFrameTime();  // returns the time last frame took (used for simulation)
};
