#pragma once

#include <filesystem>
#include <string>
#include <array>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <errors.h>

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

template<typename T>
concept Hashable = requires(T t) {
    { std::hash<T>{}(t) };
};

// to store hashable objects in a queue so there are no repeats
template<Hashable T>
class UniqueDeque : public std::deque<T> {
    // TODO: implement the other functions
public:
    template<typename... Args>
    void emplace_back(Args&&... args) {
        T new_object(std::forward<Args>(args)...);
        // emplace .second tells if emplacement was successful
        if (unique_map_.emplace(newobject, std::deque<T>::size()).second) {
            std::deque<T>::emplace_back(std::move(newobject));
        }
    }

    void push_back(T const& object) {
        if (unique_map_.emplace(object, std::deque<T>::size()).second) {
            std::deque<T>::push_back(object);
        }
    }

    void push_back(T&& object) {
        if (unique_map_.emplace(std::move(object), std::deque<T>::size()).second) {
            std::deque<T>::push_back(std::move(object));
        }
    }

    void insert(typename std::deque<T>::const_iterator position, T const& object) {
        std::deque<T>::insert(position, object);
        unique_map_[object] = std::distance(std::deque<T>::begin(), position));
    }

    void insert(size_t index, T const& object) {
        typename std::deque<T>::const_iterator pos = std::deque<T>::cbegin() + index;
        insert(pos, object);
    }

    void push_front(T const& object){
        insert(0, object);
    }
    
    void pop_front(){
        T& object = std::deque<T>::front();
        unique_map_.erase(object);
        std::deque<T>::pop_front();
    }

    void pop_back(){
        T& object = std::deque<T>::back();
        unique_map_.erase(object);
        std::deque<T>::pop_back();
    }

    void erase(size_t index) {
        T& object = std::deque<T>::at(index);
        unique_map_.erase(object);
        std::deque<T>::erase(std::deque<T>::begin() + index);
    }

    void erase(T const& object) {
        auto it = unique_map_.find(object);
        if (it != unique_map_.end()) {
            std::deque<T>::erase(std::deque<T>::begin() + it->second);
            unique_map_.erase(it);
        }
    }
    // const only because if they modify it then the map would have to be updated
    T const& operator[](size_t index) const {
        return std::deque<T>::at(index);
    }
    // deleted non-const []
    template<typename U = T>
    typename std::enable_if<!std::is_const<U>::value, T&>::type
        operator[](size_t index) = delete;

    // Modify an item at a specific index
    void modify(size_t index, T const& new_object) {
        auto it = std::next(std::deque<T>::begin(), index);
        T old_item = *it;
        *it = new_object;
        // Update map if the item has changed
        if (old_item != new_object) {
            unique_map_.erase(old_item);
            unique_map_[new_object] = index;
        }
    }

    void modify(size_t index, T&& new_object) {
        auto it = std::next(std::deque<T>::begin(), index);
        T old_item = std::move(*it);
        *it = std::move(new_object);
        // Update map if the item has changed
        if (old_item != *it) {
            unique_map_.erase(old_item);
            unique_map_[*it] = index;
        }
    }

    typename std::deque<T>::const_iterator find(T const& item) const {
        auto it = uniqueMap_.find(item);
        if (it != uniqueMap_.end()) {
            return std::deque<T>::begin() + it->second;
        }
        return std::deque<T>::end();
    }

    size_t get_index(T const& item) const {
        auto it = uniqueMap_.find(item);
        if (it != uniqueMap_.end()) {
            return it->second;
        }
        // indicate not found by returning size of deque
        return std::deque<T>::size();
    }

private:
    // object to index
    std::unordered_map<T, size_t> unique_map_;
};