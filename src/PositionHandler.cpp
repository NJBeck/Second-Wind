#include "PositionHandler.h"

#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;

std::vector<unsigned long> PositionHandler::EntitiesInRange(double start, double stop, vector<EntityPos> positions)
{
    vector<unsigned long> results;
    auto index = FirstPos(start, positions);
    // if index is -1 then left is larger than any in xPositions
    if (index == -1) { return results; }

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

vector<unsigned long> PositionHandler::EntitiesInRanges(double left, double right, double bottom, double top) {

    vector<unsigned long> entitiesInRangeY = EntitiesInRange(bottom, top, yPositions);
    vector<unsigned long> result;
    for (auto& ent : entitiesInRangeY) {
        double entXPos = xPositions[Index[ent].xIndex].Pos;
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
    result.xPos = xPositions[Index[handle].yIndex].Pos;
    result.yPos = yPositions[Index[handle].yIndex].Pos;
    return result;
}

long PositionHandler::FirstPos(double input, vector<EntityPos> vec) {

    unsigned long int prev = vec.size();
    unsigned long int mid = prev / 2;

    if (vec.empty()) { return 0; }

    // case where input is larger than any in vec
    if (input > vec.back().Pos) { return -1; }

    // case where input is smaller than any in vec
    if (input <= vec[0].Pos) { return 0; }

    // we're looking for where the midpoint is larger than input and the previous one is smaller
    while (!(
            (vec[mid].Pos >= input) 
         && (vec[mid - 1].Pos <= input)))
        // means every loop either mid point is smaller than input
        // or mid - 1 is larger than input
    {
        // store the mid to assign it to prev
        auto temp = mid;
        // half of the distance between the last midpoint and the new one (rounded up)
        unsigned long diff = ceil(abs((long)(mid - prev)) / 2.0);

        if (vec[mid].Pos >= input) {
            mid = mid - diff;
            prev = temp;
        }
        else {
            mid = mid + diff;
            prev = temp;
        }
    }
    return mid;
}

void PositionHandler::ChangePos(unsigned long handle, Pos vec)
{
    // swaps the handle to the new appropriate position in xPositions and yPositions and updates Index

    // could be made more efficient by saving the changes of indices for the positionss
    // then adding them to the appropriate entities in Index hash map all at once
    // sorting could also be deferred

    auto Xindex = Index[handle].xIndex;
    auto Yindex = Index[handle].yIndex;
    auto newXPos = xPositions[Xindex].Pos + vec.xPos;
    auto newYPos = yPositions[Yindex].Pos + vec.yPos;
    xPositions[Xindex].Pos = newXPos;
    yPositions[Yindex].Pos = newYPos;
    // std::cout << "moving entity: " << handle << " to " << newXPos << ", " << newYPos << endl;

    if (vec.xPos != 0) {
        if (newXPos < xPositions[Xindex].Pos) {
            // binary search for where it now belongs
            // could be made faster by overloading Pos
            // to work with iterators just to where the original index was
            auto newIndex = FirstPos(newXPos, xPositions);

            // nextIndex is index of element to the left which we will be swapping with
            // eg: newIndex, ... , nextIndex, Xindex, ... 
            long nextIndex = Xindex - 1;
            // keep swapping until element is in its proper place
            while (newIndex <= nextIndex) {
                // index for elements being swapped back in vector
                // should be incremented in the hashmap and vice versa
                // for the one being swapped forward
                ++Index[xPositions[nextIndex].ID].xIndex;
                --Index[xPositions[Xindex].ID].xIndex;
                swap(xPositions[nextIndex], xPositions[Xindex]);

                --Xindex;
                --nextIndex;
            }
        }
        else {
            auto newIndex = FirstPos(newXPos, xPositions);
            // if firstpos returns -1 then newxPos is bigger than any in xPositions
            if (newIndex == -1) { newIndex = xPositions.size() - 1; }

            // nextIndex is index of element to the right which we will be swapping with
            long nextIndex = Xindex + 1;
            // keep swapping until element is in its new place
            while (newIndex >= nextIndex) {
                --Index[xPositions[nextIndex].ID].xIndex;
                ++Index[xPositions[Xindex].ID].xIndex;
                swap(xPositions[nextIndex], xPositions[Xindex]);

                ++Xindex;
                ++nextIndex;
            }
        }
    }
    // now do same for Y
    if (vec.yPos != 0) {
        if (newYPos < yPositions[Yindex].Pos) {
            // binary search for where it now belongs
            // could be made faster by overloading Pos
            // to work with iterators just to where the original index was
            auto newIndex = FirstPos(newYPos, yPositions);

            // nextIndex is index of element to the left which we will be swapping with
            // eg: newIndex, ... , nextIndex, Xindex, ... 
            long nextIndex = Yindex - 1;
            // keep swapping until element is in its proper place
            while (newIndex <= nextIndex) {
                // index for elements being swapped back in vector
                // should be incremented in the hashmap and vice versa
                // for the one being swapped forward
                ++Index[yPositions[nextIndex].ID].yIndex;
                --Index[yPositions[Yindex].ID].yIndex;
                swap(yPositions[nextIndex], yPositions[Yindex]);

                --Yindex;
                --nextIndex;
            }
        }
        else {
            auto newIndex = FirstPos(newYPos, yPositions);
            // if firstpos returns -1 then newyPos is bigger than any in yPositions
            if (newIndex == -1) { newIndex = yPositions.size() - 1; }

            // nextIndex is index of element to the right which we will be swapping with
            auto nextIndex = Yindex + 1;
            // keep swapping until element is in its new place
            while (newIndex >= nextIndex) {
                --Index[yPositions[nextIndex].ID].yIndex;
                ++Index[yPositions[Yindex].ID].yIndex;
                swap(yPositions[nextIndex], yPositions[Yindex]);

                ++Yindex;
                ++nextIndex;
            }
        }
    }
}

void PositionHandler::add(unsigned long handle, Pos posn)
{
    // find iterators to point to insert the new values
    auto Xindex = FirstPos(posn.xPos, xPositions);
    auto Yindex = FirstPos(posn.yPos, yPositions);
    //std::cout << "inserting " << posn.xPos << " into " << Xindex << endl;
    //std::cout << "inserting " << posn.yPos << " into " << Yindex << endl;

    if (Xindex != -1) {
        // update index in Index and insert into position vector
        Index[handle].xIndex = Xindex;
        vector<EntityPos>::iterator it = xPositions.begin() + Xindex;
        it = xPositions.insert(it, { handle, posn.xPos });
        // now we have to tell Index to increment all the indices of the following entities after insertion
        for (it = it + 1;  it != xPositions.end(); ++it) {
            ++Index[it->ID].xIndex;
        }
    }
    // if Xindex was -1 then xPos was bigger than any in the list
    else {
        xPositions.push_back({ handle, posn.xPos });
        Index[handle].xIndex = xPositions.size() - 1;
    }
    //for (auto& item : Index) {
    //    std::cout << item.first << " " << item.second.xIndex << endl;
    //}
    if (Yindex != -1) {
        Index[handle].yIndex = Yindex;
        vector<EntityPos>::iterator it = yPositions.begin() + Yindex;
        it = yPositions.insert(it, { handle, posn.yPos });
        // now we have to tell Index to increment all the indices of the following entities after insertion
        for (it = it + 1; it != yPositions.end(); ++it) {
            ++Index[it->ID].yIndex;
        }
    }
    // if Yindex was -1 then yPos was bigger than any in the list
    else {
        yPositions.push_back({ handle, posn.yPos });
        Index[handle].yIndex = yPositions.size() - 1;
    }
}

void PositionHandler::del(unsigned long)
{
}
