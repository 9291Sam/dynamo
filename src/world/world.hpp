#ifndef SRC_WORLD_WORLD_HPP
#define SRC_WORLD_WORLD_HPP

#include <cstdint>
#include <vector>
#include <unordered_set>
#include <ranges>

#include <render/renderer.hpp>


namespace world
{
    class World
    {
    public:
        World(const render::Renderer&);
        ~World()                       = default;

        World(const World&)            = delete;
        World(World&&)                 = delete;
        World& operator=(const World&) = delete;
        World& operator=(World&&)      = delete;

        void tick();
        // bool contains(const Object&) const;
        [[nodiscard]] const std::vector<render::Object>& getObjects() const;


    public:
        std::vector<render::Object> objects;
    };
}

#endif // SRC_WORLD_WORLD_HPP