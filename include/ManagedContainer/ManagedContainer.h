#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>

template<class T>
class ManagedContainer{
    // maps handles to their index in container_
    std::unordered_map<size_t, size_t> map_;

    // contains the indices of freed objects in container_
    std::unordered_set<size_t> free_;   

    std::vector<T> container_;

    // ratio of freed/container.size() before container is rebuilt
    double load_limit_;

    void shrink(); // rebuilds container without freed elements
    public:
    ManagedContainer(double limit = .5);

    // inserts object T with specified handle
    void insert(size_t const handle, T const);

    // erase object with this handle
    void erase(size_t const handle);

    T& get(size_t handle);
};

template<class T>
ManagedContainer<T>::ManagedContainer(double limit): load_limit_(limit){}

template<class T>
void ManagedContainer<T>::insert(size_t handle, T const thing){
    if(!free_.empty()){
        auto index = *(free_.begin());
        map_[handle] = index;
        container_[index] = thing;
        free_.erase(index);
    }else{
        container_.push_back(thing);
        map_[handle] = container_.size() - 1;
    }
}

template<class T>
void ManagedContainer<T>::erase(size_t handle){
    auto found = map_.find(handle);
    if(found != map_.end()){
        free_.emplace(found->second);
        map_.erase(found);
        if(free_.size() / container_.size() > load_limit_)
            shrink();
    }else{
        throw std::range_error("handle not found");
    }
}

template<class T>
void ManagedContainer<T>::shrink(){
    size_t index = 0;
    std::vector<T> new_container;
    new_container.reserve(map_.size());
    for(auto& handle : map_){
        new_container[index] = container_[handle.second];
        handle.second = index;
    }
    container_ = new_container;
    free_.clear();
}

template<class T>
T& ManagedContainer<T>::get(size_t handle){
    auto found = map_.find(handle);
    if(found != map_.end()){
        return container_[found->second];
    }else{
        throw std::range_error("handle not found");
    }
}

