#pragma once

#include <filesystem>
#include <string>
#include <array>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

#include "glad/glad.h"
#include "globals.h"
#include "SDL2/SDL.h"

#define glCheckError() utility::glCheckError_(__FILE__, __LINE__) 

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

    inline void Log(std::string const& handler, 
                    std::string const& action, 
                    std::string const& target) 
    {
        std::cout << handler + " error when " + action + " " + target << std::endl;
    }

    template<typename T, typename U>
    inline bool InUMap(T const& key, 
                std::unordered_map<T, U> const& umap) 
    {
        if (umap.find(key) == umap.end()) return false;
        else return true;
    }

    template<typename T>
    inline bool InUSet(T const& key,
                std::unordered_set<T> const& uset)
    {
        if (uset.find(key) == uset.end()) return false;
        else return true;
    }
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

// for storing ptrs to different matrices in containers
template<typename T>
class MatrixBase {
public:
    virtual ~MatrixBase() = default;
    virtual T& operator()(u32 row, u32 col) = 0;
};

template <typename T>
class Matrix : public MatrixBase<T>{
public:
    std::vector< std::vector<T>> data;

    Matrix(u32 rows, u32 cols): data(rows, std::vector<T>(cols, T())){}

    T& operator()(u32 row, u32 col) override {
        return data[row][col];
    }
};