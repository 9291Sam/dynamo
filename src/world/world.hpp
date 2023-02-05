#pragma once

#ifndef SRC_WORLD_WORLD_HPP
#define SRC_WORLD_WORLD_HPP

#include <vector>
#include <cstdint>
#include <unordered_set>

#include "object.hpp"

class World
{
public:
    World();
    ~World();

    World(const World&)            = delete;
    World(World&&)                 = delete;
    World& operator=(const World&) = delete;
    World& operator=(World&&)      = delete;

    void tick();
    bool contains(const Object&) const;


public:
    std::unordered_set<Object> objects;
};

#endif // SRC_WORLD_WORLD_HPP