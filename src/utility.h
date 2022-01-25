#pragma once

#include <filesystem>
#include <string>
#include <tuple>

#include "glad/glad.h"
#include "SDL2/SDL.h"


namespace utility {
    // gives the absolute path to the asset from the relative path in /data/ folder
    std::string getDataPath(std::string);

    void InitGL(SDL_Window** , int width, int height);

    GLenum glCheckError_(const char* file, int line);

    template<typename T>
    class TupleHash;

    template<typename... TTypes>
    class TupleHash<std::tuple<TTypes...>>
    {
    private:
        typedef std::tuple<TTypes...> TTuple;

        // hash for empty tuple to end recursion
        template<int num_params>
        size_t operator()(TTuple const& value) const { return 0; }

        template<int num_params, typename THead, typename... TTail>
        size_t operator()(TTuple const& value) const
        {
            // how far into the tuple we are
            constexpr int index = num_params - sizeof...(TTail) - 1;
            auto old_hash = std::hash<THead>()(std::get<index>(value));
            // hash_combine shamelessly stolen from boost
            return old_hash ^ (operator() < num_params, TTail... > (value)+
                0x9e3779b9 + (old_hash << 6) + (old_hash >> 2));
        }

    public:
        size_t operator()(TTuple const& value) const
        {
            return operator() < sizeof...(TTypes), TTypes... > (value);
        }
    };
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
