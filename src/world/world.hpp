#ifndef SRC_WORLD_WORLD_HPP
#define SRC_WORLD_WORLD_HPP

#include <cstdint>
#include <vector>
#include <set>
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

        [[nodiscard]] const std::vector<render::Renderer::PipelinedObject>& getObjects() const;

        void tick();

    private:

        std::vector<render::Renderer::PipelinedObject> objects;
    };
}

#endif // SRC_WORLD_WORLD_HPP