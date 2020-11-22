#include "handlers/PositionHandler.h"


#include <iostream>

using namespace std;

std::vector<unsigned long> PositionHandler::_InRange(double start, double stop, direction dir)
{
    vector<unsigned long> results;
    auto index = FirstPos(start, dir);
    // if index is -1 then left is larger than any in xPositions
    if (index == -1) { return results; }

    vector<EntityPos>* _positions;
    if (dir == direction::y) _positions = &yPositions;
    else _positions = &xPositions;

    auto& positions = *_positions;
    // now iterate through the list until the value is greater than "right"
    while (positions[index].Pos < stop){
        results.push_back(positions[index].ID);

        // if that's the end of the list we break
        if (index == (positions.size() - 1)) { break; }
        //otherwise increment and continue
        else {
            ++index;
        }
    }
    return results;
}

vector<unsigned long> PositionHandler::InRange(double left, double right, double bottom, double top) {

    vector<unsigned long> entitiesInRangeY = _InRange(bottom, top, direction::y);
    vector<unsigned long> result;
    for (auto& ent : entitiesInRangeY) {
        double entXPos = xPositions[_xIndex[ent]].Pos;
        if (entXPos > left && entXPos < right) {
            result.push_back(ent);
        }
    }
    reverse(result.begin(), result.end());
    return result;
}

Pos PositionHandler::GetPos(unsigned long handle)
{
    Pos result;
    result.xPos = xPositions[_xIndex[handle]].Pos;
    result.yPos = yPositions[_yIndex[handle]].Pos;
    return result;
}

long PositionHandler::FirstPos(double input, direction dir) {
    vector<EntityPos>* _vec;
    if (dir == direction::y) { _vec = &yPositions; }
    else { _vec = &xPositions; }

    vector<EntityPos>& vec = *_vec;

    unsigned long left = 0;
    unsigned long right = vec.size() - 1;
    unsigned long mid = (left + right) / 2;

    if (vec.empty()) { return 0; }

    // case where input is larger than any in vec
    if (input > vec.back().Pos) { return -1; }

    // case where input is smaller than any in vec
    if (input <= vec[0].Pos) { return 0; }

    // we're looking for where the midpoint is larger than input and the previous one is smaller
    while (!
            ((vec[mid].Pos >= input)
         && (vec[mid - 1].Pos <= input))
          )
    {
        if (vec[mid].Pos < input) { left = mid + 1; }
        else { right = mid - 1; }
        
        mid = (left + right) / 2;
    }
    return mid;

}
void PositionHandler::_changepos(unsigned long handle, double posn, direction dir) {
    // swaps the handle to the new appropriate position in xPositions and yPositions and updates Index

    // could be made more efficient by saving the changes of indices for the positions
    // then adding them to the appropriate entities in Index hash map all at once
    // sorting could also be deferred
    long index;                                     // index of pos in vector before transform
    double newPos;                                  // the sum of posn and the old pos value

    vector<EntityPos>* positions;
    unordered_map<unsigned long, unsigned long>* indexMap;
    if (dir == direction::y) {
        index = _yIndex[handle];
        newPos = yPositions[index].Pos + posn;
        positions = &yPositions;
        indexMap = &_yIndex;
    }
    else {
        index = _xIndex[handle];
        newPos = xPositions[index].Pos + posn;
        positions = &xPositions;
        indexMap = &_xIndex;
    }

    if (posn != 0) {
        auto newIndex = FirstPos(newPos, dir);

        if (newPos < (*positions)[index].Pos) {
            // binary search for where it now belongs
            // could be made faster by overloading Pos
            // to work with iterators just to where the original index was

            // nextIndex is index of element to the left which we will be swapping with
            // eg: newIndex, ... , nextIndex, Xindex, ... 
            long nextIndex = index - 1;

            // keep swapping until element is in its proper place
            while (newIndex <= nextIndex) {
                ++(*indexMap)[(*positions)[nextIndex].ID];
                --(*indexMap)[(*positions)[index].ID];
                swap((*positions)[nextIndex], (*positions)[index]);

                --index;
                --nextIndex;
            }

        }
        else {
            
            // if firstpos returns -1 then newxPos is bigger than any in xPositions
            if (newIndex == -1) { newIndex = positions->size() - 1; }

            long nextIndex = index + 1;
            while (newIndex >= nextIndex) {
                // index for elements being swapped back in vector
                // should be incremented in the hashmap and vice versa
                // for the one being swapped forward
                --(*indexMap)[(*positions)[nextIndex].ID];
                ++(*indexMap)[(*positions)[index].ID];
                swap((*positions)[nextIndex], (*positions)[index]);

                ++index;
                ++nextIndex;
            }
        }
    }
}

void PositionHandler::ChangePos(unsigned long handle, Pos vec)
{
    _changepos(handle, vec.xPos, direction::x);
    _changepos(handle, vec.yPos, direction::y);
}

void PositionHandler::add(unsigned long handle, Pos posn)
{
    _addpos(handle, posn.xPos, direction::x);
    _addpos(handle, posn.yPos, direction::y);
}

void PositionHandler::_addpos(unsigned long handle, double posn, direction dir) {
    auto _index = FirstPos(posn, dir);

    vector<EntityPos>* positions;
    unordered_map<unsigned long, unsigned long>* index;
   
    if(dir == direction::y) { 
        positions = &yPositions; 
        index = &_yIndex;
    }
    else {
        positions = &xPositions;
        index = &_xIndex;
    }

    if (_index != -1) {

        vector<EntityPos>::iterator it = positions->begin() + _index;
        positions->insert(it, { handle, posn });
        // now we have to tell Index to increment all the indices of the following entities after insertion
        // update index in Index and insert into position vector
        (*index)[handle] = _index;
        for (unsigned i = _index + 1; i < positions->size(); ++i) {
            ++(*index)[(*positions)[i].ID];
        }

    }
    // if _index was -1 then posn was bigger than any in the list
    else {
        positions->push_back({ handle, posn });
        (*index)[handle] = positions->size() - 1;
    }
}

void PositionHandler::del(unsigned long)
{
}
