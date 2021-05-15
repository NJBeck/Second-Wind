#include <stdio.h>
#include <stdexcept>
#include <algorithm>

#include "handlers/PositionHandler.h"

using namespace std;


Pos PositionHandler::GetPos(entity::ID const handle)
{
    return map_[handle].pos;
}

void PositionHandler::ChangePos(entity::ID const ent_id, 
                                double const x_vec, double const y_vec)
{
    auto old_indices = map_[ent_id].quadtree_indices;
    Pos newPos{ map_[ent_id].pos.xPos + x_vec, map_[ent_id].pos.yPos + y_vec };
    Dimensions dims{ map_[ent_id].dimensions };
    
    tree_.Query(newPos, dims, tree_.origin_);

    for (auto& index : tree_.query_result_) {
        // if it has moved into a new leaf
        if (old_indices.find(index.index) == old_indices.end()) {

        }
    }
    //Pos newPos{ map_[ent_id].pos.xPos + x_vec, map_[ent_id].pos.yPos + y_vec };
    //auto current_indices = map_[ent_id].quadtree_indices;
    //
    //QuadTree::EntPosDim current_EntPosDim { ent_id, map_[ent_id].dimensions, map_[ent_id].pos };
    //auto indices = map_[ent_id].quadtree_indices;
    //for (auto index : indices) {
    //    auto& leafSet = tree_.quadtree_[index].entities;

    //    auto it = leafSet.find(current_EntPosDim);
    //    if (it == leafSet.end()) {
    //        char error_string[256];
    //        sprintf(error_string, "entity at (%G, %G) not found in leaf", 
    //                current_EntPosDim.pos.xPos, current_EntPosDim.pos.yPos);
    //        throw std::runtime_error(error_string);
    //    }

    //    auto newIter = leafSet.erase(it);

    //    leafSet.emplace_hint(newIter, ent_id, current_EntPosDim.dimensions,
    //                         newPos);
    //    // check to see if the entity has moved out of its current quadrants
    //}
    //map_[ent_id].pos = newPos;
}

void PositionHandler::Add(entity::ID const handle, Pos const position, 
                          Dimensions const dim)
{
    PosData newData;
    newData.pos = position;
    newData.dimensions = dim;
    newData.quadtree_indices = tree_.Add(handle, position, dim);
    map_[handle] = newData;
}

void PositionHandler::Remove(entity::ID const)
{
}

QuadTree::QuadTree(double const xmin, double const xmax, double const ymin,
                   double const ymax) 
    : x_min_(xmin), x_max_(xmax), y_min_(ymin), y_max_(ymax), entity_max_(3) {
    origin_ = { 0, {(x_max_ - x_min_), (y_max_ - y_min_)}, { x_min_, y_min_} };
}

void QuadTree::Query(Pos const posn, Dimensions const dims, 
                     NodeInfo& current_node)
{
    query_result_.clear();
    auto current_dims = current_node.dimensions;
    auto current_pos = current_node.pos;
    auto current_index = current_node.index;

    while (quadtree_[current_index].next != 0) {
        double xmin = current_pos.xPos;
        double xmax = xmin + current_dims.width;
        double ymin = current_pos.yPos;
        double ymax = ymin + current_dims.height;
        auto quadrants = FindQuad(posn, dims, current_pos, current_dims);

        double newWidth = (xmax - xmin) / 2;
        double newHeight = (ymax - ymin) / 2;
        current_node.dimensions = { newWidth, newHeight };
        Pos newQuadPos;
        if (quadrants[0]) {
            // point is in North West -> yPos changes
            newQuadPos.xPos = xmin;
            newQuadPos.yPos = ymin + newHeight;
            // modify info with new index and Pos
            current_node.index = quadtree_[current_index].next;
            current_node.pos = newQuadPos;
            return Query(posn, dims, current_node);
        }
        if (quadrants[1]) {
            // point is in North East -> xmin and ymin increase
            newQuadPos.xPos = xmin + newWidth;
            newQuadPos.yPos = ymin + newHeight;

            // because .next points to the NorthWest node and nodes go in 
            // clockwise order NE is 1 past the .next index
            current_node.index = quadtree_[current_index].next + 1;
            current_node.pos = newQuadPos;
            return Query(posn, dims, current_node);
            
        }
        if (quadrants[2]) {
            // point is in South East -> xmin and ymax change
            newQuadPos.yPos = ymin;
            newQuadPos.xPos = xmin + newWidth;

            current_node.index = quadtree_[current_index].next + 2;
            current_node.pos = newQuadPos;
            return Query(posn, dims, current_node);
        }
        if (quadrants[3]) {
            // point is in South West -> xmax and ymax change
            newQuadPos.yPos = ymin;
            newQuadPos.xPos = xmin;

            current_node.index = quadtree_[current_index].next + 3;
            current_node.pos = newQuadPos;
            return Query(posn, dims, current_node);
        }



    }
    query_result_.push_back(current_node);
    
}

array<bool, 4> QuadTree::FindQuad(Pos const entity_pos,
                                  Dimensions const entity_dim,
                                  Pos const quadrant_pos,
                                  Dimensions const quadrant_dim)
{
    array<bool, 4> quads;
    // setting the size of the quad of the whole area to be divided
    double xmin = quadrant_pos.xPos;
    double xmax = xmin + quadrant_dim.width;
    double ymin = quadrant_pos.yPos;
    double ymax = ymin + quadrant_dim.height;
    double xmid = xmin + (xmax - xmin) / 2;
    double ymid = ymin + (ymax - ymin) / 2;

    bool left = IntervalTest(entity_pos.xPos, entity_pos.xPos + entity_dim.width, xmin, xmid);
    bool right = IntervalTest(entity_pos.xPos, entity_pos.xPos + entity_dim.width, xmid, xmax);
    bool bottom = IntervalTest(entity_pos.yPos, entity_pos.yPos + entity_dim.height, ymin, ymid);
    bool top = IntervalTest(entity_pos.yPos, entity_pos.yPos + entity_dim.height, ymid, ymax);

    quads[0] = left && top;
    quads[1] = right && top;
    quads[2] = right && bottom;
    quads[3] = left && bottom;
    return quads;

}

bool QuadTree::IntervalTest(double const start1, double const end1, 
                            double const start2, double const end2) {
    if (start1 >= end2) return false;
    if (end1 <= start2) return false;
    return true;
}


vector<uint32_t> QuadTree::Add(entity::ID handle, Pos const position,
                               Dimensions const dims)
{
    
    vector<uint32_t> indices; // indices into quadtree_ vector for the entity


    Query(position, dims, origin_);

    QuadTree::EntPosDim ent{ handle, dims, position };
    for (auto& leaf_info : query_result_) {
        auto& leaf = quadtree_[leaf_info.index];
        auto& leafSet = leaf.entities;
        // if the set is full and the max depth hasnt been reached
        // we turn current leaf into a branch and split 4 new leaves
        bool limit_reached = (leaf_info.dimensions.height - quad_limit_) < .1 ||
                             (leaf_info.dimensions.width - quad_limit_) < .1;
        leafSet.insert(ent);
        if (leafSet.size() > entity_max_ && !limit_reached) {
            quadtree_.reserve(quadtree_.size() + 4);
            auto i = quadtree_.size();
            auto j = i + 4;
            for (i; j; ++i) {
                ListNode temp;
                temp.next = 0;
                quadtree_.emplace_back(temp);
            }
            for (auto& ent_in_leaf : leafSet) {
                auto quadrants = FindQuad(ent_in_leaf.pos, 
                                          ent_in_leaf.dimensions, 
                                          leaf_info.pos, 
                                          leaf_info.dimensions);

                if (quadrants[0]) {
                    quadtree_[i].entities.insert(ent_in_leaf);
                    indices.push_back(i);
                }
                if (quadrants[1]) {
                    quadtree_[i + 1].entities.insert(ent_in_leaf);
                    indices.push_back(i + 1);
                }
                if (quadrants[2]) {
                    quadtree_[i + 2].entities.insert(ent_in_leaf);
                    indices.push_back(i + 2);
                }
                if (quadrants[3]) {
                    quadtree_[i + 3].entities.insert(ent_in_leaf);
                    indices.push_back(i + 3);
                }
            }
            leaf.next = i;
        }
        else {
            indices.push_back(leaf_info.index);
        }
    }
    return indices;
}

bool operator<(QuadTree::EntPosDim& const lhs, QuadTree::EntPosDim& const rhs)
{
    auto lhs_yval = lhs.pos.yPos + lhs.dimensions.height;
    auto rhs_yval = rhs.pos.yPos + rhs.dimensions.height;
    auto lhs_xval = lhs.pos.xPos + lhs.dimensions.width;
    auto rhs_xval = rhs.pos.xPos + rhs.dimensions.width;
    if (lhs_yval > rhs_yval) return true;
    else {
        if (lhs_xval > rhs_xval) return true;
        else {
            if (lhs.ID < rhs.ID) return true;
            else return false;
        }
    }
}
bool operator<(QuadTree::NodeInfo& const lhs, QuadTree::NodeInfo& const rhs) {
    return lhs.index < rhs.index;
}
