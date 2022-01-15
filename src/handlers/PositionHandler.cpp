// TODO: add exceptions for invalid accesses
#include <exception>
#include <string>
#include <iostream>
#include <utility>

#include "handlers/PositionHandler.h"

using namespace std;

PositionHandler::PositionHandler(Quad boundaries) 
    : map_boundaries_(boundaries), entity_max_(3), resolution_(1.0) {
    TreeNode origin = { 0, { 0,0,0,0 }, { 0,1,2,3 } };
    quadtree_.emplace_back(origin);
    // insert the 4 starting leaf_data_ nodes
    MakeNewLeaf(map_boundaries_, Quadrants::NW, 0);
    MakeNewLeaf(map_boundaries_, Quadrants::NE, 0);
    MakeNewLeaf(map_boundaries_, Quadrants::SE, 0);
    MakeNewLeaf(map_boundaries_, Quadrants::SW, 0);
}

void PositionHandler::Add(EntityID const handle, Quad const quad)
{
    Insert(handle, quad, map_boundaries_, 0);
}


void PositionHandler::Move(EntityID const handle, Pos const vec) {
    auto found = index_.find(handle);
    if (found != index_.end()) {
        // indices for in leaf_data_ to remove the entity from
        unordered_set<uint32_t> to_remove;
        // indices in quadtree_ to add the entity to
        // and the Quad of the containing node
        vector<tuple<uint32_t, Quad> > to_add;

        EntityData& ent_data = found->second;
        // first we store the old position for comparison then update
        Quad old_quad = ent_data.quad;
        ent_data.quad.xPos += vec.xPos;
        ent_data.quad.yPos += vec.yPos;
        // for every leaf it's in we check if a boundary was crossed
        Quad& new_quad = ent_data.quad;
        for (auto& leaf_index : ent_data.indices) {
            LeafData& leaf = leaf_data_[leaf_index];
            if (CrossBoundary(old_quad, vec, leaf.quad)) {
                // if so then traverse back up the quadtree until
                // the quad is fully contained in a node
                // then add/remove from leaves as necessary
                uint32_t& node_index = leaf.quadtree_index;
                // boundaries for the node containing the leaf
                Quad parent_quad = ParentQuad(leaf.quad,
                                              leaf.quadrant);
                // we need to go up the tree until the entity is fully
                // contained in a node
                while (!FullyContained(new_quad, parent_quad)) {
                    parent_quad = ParentQuad(parent_quad,
                                             QuadrantOfParent(node_index));
                    node_index = quadtree_[node_index].previous;
                }
                // test if the movement removed the entity from the leaf
                auto intersection = FindQuad(new_quad, leaf.quad);
                if (!(intersection[0] || intersection[1] ||
                      intersection[2] || intersection[3])) {
                    to_remove.emplace(leaf_index);
                }
                to_add.emplace_back(node_index, parent_quad);
            }
        }
        for (auto& element : to_add) {
            uint32_t& elem_index = get<0>(element);
            Quad& elem_quad = get<1>(element);
            Insert(handle, new_quad, elem_quad, elem_index);
        }
        RemoveFromLeaf(handle, to_remove);
    }
    else {
        string err = to_string(handle) + " not found when trying to move";
        throw runtime_error(err);
    }
}

void PositionHandler::Remove(EntityID const handle) {
    RemoveFromLeaf(handle, index_[handle].indices);
    index_.erase(handle);
}

PositionHandler::Quad PositionHandler::GetPos(EntityID const handle) const
{
    auto found = index_.find(handle);
    if (found != index_.end()) {
        return found->second.quad;
    }
    else {
        string err = handle + "not found when accessing position";
        throw runtime_error(err);
    }
}

set<PositionHandler::EntityQuad, PositionHandler::YDescendingOrder> 
PositionHandler::EntitiesInArea(Quad const area) const{
    set<EntityQuad, YDescendingOrder> results;

    unordered_set<uint32_t> QueryResults = Query(area);
    unordered_set<EntityID> EntityResults;

    for (auto& index : QueryResults) {
        auto& entities_in_leaf = leaf_data_[index].entities;
        for (EntityID handle : entities_in_leaf) {
            EntityResults.emplace(handle);
        }
    }
    for (auto& handle : EntityResults) {
        Quad ent_quad;
        auto found = index_.find(handle);
        if (found != index_.end()) {
            ent_quad = found->second.quad;
            results.insert({ handle, ent_quad });
        }
        else {
            string err = handle + "not found when accessing Quad position";
            throw runtime_error(err);
        }
    }
    return results;
}

/*
// HELPER FUNCTIONS START
*/

//PositionHandler::Quad PositionHandler::CalculateQuadrant(vector<Quadrants>& quadrants) {
//    Quad quadrant;
//    quadrant.height = y_max_ - y_min_;
//    quadrant.width = x_max_ - x_min_;
//    quadrant.xPos = x_min_;
//    quadrant.yPos = y_min_;
//    for (auto& i : quadrants) {
//        quadrant.height = quadrant.height / 2;
//        quadrant.width = quadrant.width / 2;
//        switch (i) {
//            case Quadrants::NW: {
//                quadrant.yPos += quadrant.height;
//                break;
//            }
//            case Quadrants::NE: {
//                quadrant.yPos += quadrant.height;
//                quadrant.xPos += quadrant.width;
//                break;
//            }
//            case Quadrants::SE: {
//                quadrant.xPos += quadrant.width;
//                break;
//            }
//            case Quadrants::SW: {
//                break;
//            }
//        }
//    }
//    return quadrant;
//}
////bool operator<(PositionHandler::EntPosDim& const lhs, PositionHandler::EntPosDim& const rhs)
////{
////    auto lhs_yval = lhs.pos.yPos + lhs.dimensions.height;
////    auto rhs_yval = rhs.pos.yPos + rhs.dimensions.height;
////    auto lhs_xval = lhs.pos.xPos + lhs.dimensions.width;
////    auto rhs_xval = rhs.pos.xPos + rhs.dimensions.width;
////    if (lhs_yval > rhs_yval) return true;
////    else {
////        if (lhs_xval > rhs_xval) return true;
////        else {
////            if (lhs.ID < rhs.ID) return true;
////            else return false;
////        }
////    }
////}
////bool operator<(PositionHandler::NodeInfo& const lhs, PositionHandler::NodeInfo& const rhs) {
////    return lhs.index < rhs.index;
////}

unordered_set<uint32_t> PositionHandler::Query(Quad const ent_quad) const
{
    return Query(ent_quad, map_boundaries_, 0);
}

unordered_set<uint32_t> PositionHandler::Query(Quad const ent_quad, 
                                               Quad init_quad, 
                                               uint32_t const start) const
{
    unordered_set<uint32_t> query_results;

    // we iterate over a quadtree_ indices and their corresponding Quads
    // removing an index when we've explored it then added its children
    unordered_map<uint32_t, Quad> todo;
    todo.emplace(start, init_quad);
    while (!todo.empty()){
        uint32_t current_index = todo.begin()->first;
        Quad current_quadrant = todo.begin()->second;
        todo.erase(todo.begin());

        // we start constructing the Quads for the children
        Quad new_quad = current_quadrant;
        // each subquadrant has the same width and height (half the parent)
        new_quad.height = current_quadrant.height / 2;
        new_quad.width = current_quadrant.width / 2;

        auto const& quad_node = quadtree_[current_index];
        // now we find which quadrants ent_quad intersects
        // add them to todo if not a leaf and continue iterating 
        auto quadrants = FindQuad(ent_quad, current_quadrant);
        if (quadrants[0]) {
            auto next_index = quad_node.next_indices[0];
            // if next == 0 then we are at a leaf
            if (next_index == 0) {
                query_results.insert(quad_node.leaf_indices[0]);
            }
            else {
                // else we add a new node to search to todo
                // quadrants[0] is NorthEast so yPos changes
                new_quad.xPos = current_quadrant.xPos;
                new_quad.yPos = current_quadrant.yPos + new_quad.height;
                todo[next_index] = new_quad;
            }
        }
        if (quadrants[1]) {
            auto next_index = quad_node.next_indices[1];
            // North West
            if (next_index == 0) {
                query_results.insert(quad_node.leaf_indices[1]);
            }
            else {
                new_quad.xPos = current_quadrant.xPos + new_quad.width;
                new_quad.yPos = current_quadrant.yPos + new_quad.height;
                todo[next_index] = new_quad;
            }
        }
        if (quadrants[2]) {
            auto next_index = quad_node.next_indices[2];
            // South East
            if (next_index == 0) {
                query_results.insert(quad_node.leaf_indices[2]);
            }
            else {
                new_quad.xPos = current_quadrant.xPos + new_quad.width;
                new_quad.yPos = current_quadrant.yPos;
                todo[next_index] = new_quad;
            }
        }
        if (quadrants[3]) {
            auto next_index = quad_node.next_indices[3];
            // South West
            if (next_index == 0) {
                query_results.insert(quad_node.leaf_indices[3]);
            }
            else {
                new_quad.xPos = current_quadrant.xPos;
                new_quad.yPos = current_quadrant.yPos;
                todo[next_index] = new_quad;
            }
        }
    }
    
    return query_results;
}

void PositionHandler::Insert(EntityID const handle, Quad const ent_quad, 
                             Quad const boundary_quad, uint32_t const start)
{
    index_[handle] = {};
    auto& entity_data = index_[handle];
    entity_data.quad = ent_quad;

    auto leafs = Query(ent_quad, boundary_quad, 0);
    unordered_map<uint32_t, unordered_set<EntityID> > todo;
    for (auto& leaf : leafs) todo[leaf].emplace(handle);

    while(!todo.empty()) {
        uint32_t const& leaf_index = todo.begin()->first;
        auto& leaf = leaf_data_[leaf_index];

        bool limit_reached = (leaf.quad.height 
                              - resolution_) < .01 ||
                             (leaf.quad.width 
                              - resolution_) < .01;
        for (auto& entity : todo.begin()->second) {
            leaf.entities.emplace(entity);
        }
        if (leaf.entities.size() > entity_max_ && !limit_reached) {
            // if the set is full and the max depth hasnt been reached
            // we turn current leaf into a branch
            // by setting its leaf_index to 0 and pushing a new TreeNode
            // onto quadtree_ and a new LeafData for each leaf in the new node
            // which it occupies then adding those indices to the list
            // we are iterating over

            // the node in quadtree_ that contains the leaf and its siblings
            auto& quadtree_node = quadtree_[leaf.quadtree_index];
            // push the new quadtree_ node and start filling it out
            auto& new_tree_node = quadtree_.emplace_back();
            new_tree_node.previous = leaf.quadtree_index;
            new_tree_node.leaf_indices = { 0,0,0,0 };
            new_tree_node.next_indices = { 0,0,0,0 };

            // update the next_indices of the quadtree_node to new leaf
            switch (leaf.quadrant) {
                case Quadrants::NW:
                    // set the next index to the newly pushed node
                    quadtree_node.next_indices[0] = leaf_data_.size() - 1;
                    break;
                case Quadrants::NE: 
                    quadtree_node.next_indices[1] = leaf_data_.size() - 1;
                    break;
                case Quadrants::SE: 
                    quadtree_node.next_indices[2] = leaf_data_.size() - 1;
                    break;
                case Quadrants::SW: 
                    quadtree_node.next_indices[3] = leaf_data_.size() - 1;
                    break;
            }
            // find the quadrants the entity exists in the new node
            Quad const& quad_pos = leaf.quad;
            // push new LeafData node onto leaf_data_ for each intersection
            LeafData new_leaf_node;
            Quad new_quad_pos;  // the position data of the new leaf node
            new_quad_pos.height = quad_pos.height / 2;
            new_quad_pos.width = quad_pos.width / 2;
            // for each entity in the leaf we need to find which new leaves
            // it belongs to then make leaf and insert entity
            for (auto& entity : leaf.entities) {
                auto quadrants = FindQuad(index_[entity].quad, quad_pos);
                if (quadrants[0]) {
                    auto new_leaf_index = new_tree_node.leaf_indices[0];
                    // if the leaf doesnt already exist we make it
                    if (new_leaf_index == 0) {
                        new_leaf_index = MakeNewLeaf(quad_pos, Quadrants::NW, 
                                                     quadtree_.size() - 1);
                    }
                    // because we've added a new leaf we put it in the set
                    todo[new_leaf_index].emplace(entity);
                    // update the new TreeNode's leaf_index
                    new_tree_node.leaf_indices[0] = new_leaf_index;
                }
                if (quadrants[1]) {
                    auto new_leaf_index = new_tree_node.leaf_indices[1];
                    if (new_leaf_index == 0) {
                        new_leaf_index = MakeNewLeaf(quad_pos, Quadrants::NE,
                                                     quadtree_.size() - 1);
                    }
                    todo[new_leaf_index].emplace(entity);
                    new_tree_node.leaf_indices[1] = new_leaf_index;
                }
                if (quadrants[2]) {
                    auto new_leaf_index = new_tree_node.leaf_indices[2];
                    if (new_leaf_index == 0) {
                        new_leaf_index = MakeNewLeaf(quad_pos, Quadrants::SE, 
                                                     quadtree_.size() - 1);
                    }
                    todo[new_leaf_index].emplace(entity);
                    new_tree_node.leaf_indices[2] = new_leaf_index;
                }
                if (quadrants[3]) {
                    auto new_leaf_index = new_tree_node.leaf_indices[3];
                    if (new_leaf_index == 0) {
                        new_leaf_index = MakeNewLeaf(quad_pos, Quadrants::SW, 
                                                     quadtree_.size() - 1);
                    }
                    todo[new_leaf_index].emplace(entity);
                    new_tree_node.leaf_indices[3] = new_leaf_index;
                }
            }
        }
        else {
            entity_data.indices.emplace(leaf_index);
        }
        todo.erase(leaf_index);
    }
}

uint32_t PositionHandler::MakeNewLeaf(Quad const& parent, Quadrants quadrant,
                                      uint32_t parent_index) {
    LeafData new_leaf_node;
    new_leaf_node.quadtree_index = parent_index;
    Quad new_quad_pos;  // the position data of the new leaf node
    new_quad_pos.height = parent.height / 2;
    new_quad_pos.width = parent.width / 2;
    switch (quadrant) {
    case Quadrants::NW: {
        new_leaf_node.quadrant = Quadrants::NW;
        new_leaf_node.quadtree_index = quadtree_.size() - 1;

        new_quad_pos.xPos = parent.xPos;
        new_quad_pos.yPos = parent.yPos + new_quad_pos.height;
        new_leaf_node.quad = new_quad_pos;

        leaf_data_.emplace_back(new_leaf_node);
        break;
    }
    case Quadrants::NE: {
        new_leaf_node.quadrant = Quadrants::NE;
        new_leaf_node.quadtree_index = quadtree_.size() - 1;

        new_quad_pos.xPos = parent.xPos + new_quad_pos.width;
        new_quad_pos.yPos = parent.yPos + new_quad_pos.height;
        new_leaf_node.quad = new_quad_pos;

        leaf_data_.emplace_back(new_leaf_node);
        break;
    }
    case Quadrants::SE: {
        new_leaf_node.quadrant = Quadrants::SE;
        new_leaf_node.quadtree_index = quadtree_.size() - 1;

        new_quad_pos.xPos = parent.xPos + new_quad_pos.width;
        new_quad_pos.yPos = parent.yPos;
        new_leaf_node.quad = new_quad_pos;

        leaf_data_.emplace_back(new_leaf_node);
        break;
    }
    case Quadrants::SW: {
        new_leaf_node.quadrant = Quadrants::SW;
        new_leaf_node.quadtree_index = quadtree_.size() - 1;

        new_quad_pos.xPos = parent.xPos;
        new_quad_pos.yPos = parent.yPos;
        new_leaf_node.quad = new_quad_pos;

        leaf_data_.emplace_back(new_leaf_node);
        break;
    }

    }
    return leaf_data_.size() - 1;
}

array<bool, 4> PositionHandler::FindQuad(Quad const entity_quad,
                                  Quad const quadrant_quad) const
{
    Pos entity_pos = entity_quad;
    Pos quadrant_pos = quadrant_quad;
    Dimensions entity_dim = entity_quad;
    Dimensions quadrant_dim = quadrant_quad;
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

bool PositionHandler::IntervalTest(double const start1, double const end1, 
                            double const start2, double const end2) const {
    if (start1 >= end2) return false;
    if (end1 <= start2) return false;
    return true;
}

bool PositionHandler::CrossBoundary(Quad const ent_quad, Pos const vec,
                                    Quad const map_quad) const {
    double map_xmin = map_quad.xPos;
    double map_xmax = map_quad.xPos + map_quad.width;
    double map_ymin = map_quad.yPos;
    double map_ymax = map_quad.yPos + map_quad.height;

    double ent_xmin = ent_quad.xPos;
    double ent_xmax = ent_quad.xPos + ent_quad.width;
    double ent_ymin = ent_quad.yPos;
    double ent_ymax = ent_quad.yPos + ent_quad.height;

    // if an edge of the entity starts in the ends up outside the map border
    bool cross_top = ent_ymax <= map_ymax && 
                    (ent_ymax + vec.yPos) > map_xmax;
    bool cross_bottom = ent_ymin >= map_ymin && 
                       (ent_ymin + vec.yPos) < map_ymin;
    bool cross_left = ent_xmin >= map_xmin && 
                     (ent_xmin + vec.xPos) < map_xmin;
    bool cross_right = ent_xmax <= map_xmax && 
                      (ent_xmax + vec.xPos) > map_xmax;

    return cross_top || cross_bottom || cross_left || cross_right;
}

void 
PositionHandler::RemoveFromLeaf(EntityID const handle,
                                unordered_set<uint32_t> const& leaf_indices) {
    // index of leaf node in quadtree_ with at least 1 (now) empty quadrant,
    // quadrant in the node, Quad of that quadrant
    using QuadNodeInfo = tuple<uint32_t, Quadrants, Quad>;
    // info for nodes in quadtree_ to have a leaf removed
    vector<QuadNodeInfo> to_remove; 

    // if the entity is the last in the node then add to list of nodes 
    // in quadtree_ to visit and modify it
    for (auto& leaf_index : leaf_indices) {
        LeafData& leaf = leaf_data_[leaf_index];
        leaf.entities.erase(handle);
        if (leaf.entities.size() == 0) {
            to_remove.emplace_back(leaf.quadtree_index, leaf.quadrant, 
                                       leaf.quad);
            // TODO: clean up deallocated leaf and quadtree nodes
        }
    }
    // TODO: maybe forward the leaf index if there is just 1 nonempty leaf left
    // to the parent instead of when all are empty

    // in the quadtree_ nodes we set the specified quadrant of TreeNode to 0
    // then check if all the leaves are now empty
    // and delete the quadtree_ node if they are
    while (!to_remove.empty()) {
        auto node_index = get<0>(to_remove[to_remove.size() - 1]);
        auto node_quadrant = get<1>(to_remove[to_remove.size() - 1]);
        auto node_quad_pos = get<2>(to_remove[to_remove.size() - 1]);
        TreeNode& node = quadtree_[node_index];
        // set the entry in leaf_indices to 0 to signify it is an empty leaf
        // THIS LEAKS THE MEMORY IN LeafData (see above TODO)
        node.leaf_indices[node_quadrant] = 0;
        // if all leaves are empty then deallocate the node
        bool delete_node = true;
        for (auto& index : node.leaf_indices) {
            if (index != 0) {
                delete_node = false;
            }
        }
        // to deallocate we go to the node's parent and set next and leaf to 0
        if (delete_node) {
            TreeNode& parent_node = quadtree_[node.previous];
            for (int i = 0; i < parent_node.next_indices.size(); ++i){
                // find the quadrant that pointed to our node
                if (parent_node.next_indices[i] == node.previous) {
                    parent_node.next_indices[i] = 0;
                    parent_node.leaf_indices[i] = 0;
                    // TODO: CLEAN UP TREENODE
                }
            }
            // now we push the parent's info back in case this operation
            // has also emptied all of its leaves
            node_quad_pos = ParentQuad(node_quad_pos, node_quadrant);
            node_quadrant = QuadrantOfParent(node_index);
            Quad parent_quad = ParentQuad(node_quad_pos, node_quadrant);
            to_remove.emplace_back(node.previous, 
                                   node_quadrant, 
                                   parent_quad);
        }
        to_remove.pop_back();
    }
}

PositionHandler::Quad 
PositionHandler::QuadOfQuadrant(Quad const q, 
                                Quadrants const corner) const {
    double new_height = q.height / 2;
    double new_width = q.width / 2;

    Quad new_quad;
    new_quad.height = new_height;
    new_quad.width = new_width;

    switch (corner) {
        case Quadrants::NW: {
            new_quad.xPos = q.xPos;
            new_quad.yPos = q.yPos + new_height;
        }
        case Quadrants::NE: {
            new_quad.xPos = q.xPos + new_width;
            new_quad.yPos = q.yPos + new_height;
        }
        case Quadrants::SE: {
            new_quad.xPos = q.xPos + new_width;
            new_quad.yPos = q.yPos;
        }
        case Quadrants::SW: {
            new_quad.xPos = q.xPos;
            new_quad.yPos = q.yPos;
        }
    }
    return new_quad;
}

PositionHandler::Quadrants 
PositionHandler::QuadrantOfQuad(Quad const parent, 
                                Quad const child) const {
    bool east = (child.xPos - parent.xPos) > .0000001;
    bool north = (child.yPos - parent.yPos) > .0000001;
    if (east) {
        if (north) return Quadrants::NE;
        else return Quadrants::SE;
    }
    else {
        if (north) return Quadrants::NW;
        else return Quadrants::SW;
    }
}

PositionHandler::Quad 
PositionHandler::ParentQuad(Quad const q, 
                            Quadrants const corner) const{
    Quad parent_quad;
    parent_quad.height = q.height * 2;
    parent_quad.width = q.width * 2;
    switch(corner) {
    case Quadrants::NW: {
        parent_quad.xPos = q.xPos;
        parent_quad.yPos = q.yPos - q.height;
        break;
    }
    case Quadrants::NE: {
        parent_quad.xPos = q.xPos - q.width;
        parent_quad.yPos = q.yPos - q.height;
        break;
    }
    case Quadrants::SE: {
        parent_quad.xPos = q.xPos - q.width;
        parent_quad.yPos = q.yPos;
        break;
    }
    case Quadrants::SW: {
        parent_quad.xPos = q.xPos;
        parent_quad.yPos = q.yPos;
        break;
    }
    }
    return parent_quad;
}

bool PositionHandler::FullyContained(Quad const inner, 
                                     Quad const outer) const
{
    bool fully_contained =
        inner.xPos >= outer.xPos && 
        inner.yPos >= outer.yPos && 
        inner.xPos + inner.width < outer.xPos + outer.width && 
        inner.yPos + inner.height < outer.yPos + outer.height;
    return fully_contained;
}

PositionHandler::Quadrants 
PositionHandler::QuadrantOfParent(uint32_t const child) const
{
    uint32_t const parent = quadtree_[child].previous;
    auto const& parent_indices = quadtree_[parent].next_indices;
    if (parent_indices[0] == child) return Quadrants::NW;
    if (parent_indices[1] == child) return Quadrants::NE;
    if (parent_indices[2] == child) return Quadrants::SE;
    if (parent_indices[3] == child) return Quadrants::SW;
}

bool 
PositionHandler::YDescendingOrder::operator()(EntityQuad const& lhs, 
                                              EntityQuad const& rhs) const
{
    return lhs.quad.yPos > rhs.quad.yPos;
}
