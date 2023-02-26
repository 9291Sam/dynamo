#include "world.hpp"

namespace world
{
    World::World(const render::Renderer& renderer)
    {
        auto [v, i] = render::Object::readVerticesFromFile("../models/gizmo.obj");
        this->objects.push_back(renderer.createObject(std::move(v), std::move(i)));
        this->objects.at(0).transform.scale = {4.0f, 4.0f, 4.0f};

        auto [b, j] = render::Object::readVerticesFromFile("../models/colored_cube.obj");
        this->objects.push_back(renderer.createObject(std::move(b), std::move(j)));
        this->objects.at(1).transform.scale = {100.0f, 100.0f, 100.0f};
        this->objects.at(1).transform.translation.y -= 120.0f;

        auto [k, l] = render::Object::readVerticesFromFile("../models/64k.obj");
        this->objects.push_back(renderer.createObject(std::move(k), std::move(l)));
        this->objects.at(2).transform.scale = {500.0f, 500.0f, 500.0f};
        this->objects.at(2).transform.translation.y += 600.0f;
    }

    const std::vector<render::Object>& World::getObjects() const
    {
        return this->objects;
    }
}

        