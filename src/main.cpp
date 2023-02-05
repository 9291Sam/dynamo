#include <iostream>

#include <sebib/seblog.hpp>
#include <render/renderer.hpp>

int main()
{
    
    seb::logLog("Dynamo started | Version: {}.{}.{}.{}",
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_PATCH,
        VERSION_TWEAK
    );

    try
    {
        render::Renderer renderer {{1200, 1200}, "Dynamo"};
        auto keyCallback = renderer.getKeyCallback();

        std::vector<render::Object> objects;

        auto [v, i] = render::Object::readVerticesFromFile("../models/gizmo.obj");
        objects.push_back(renderer.createObject(std::move(v), std::move(i)));
        objects.at(0).transform.scale = {4.0f, 4.0f, 4.0f};

        auto [b, j] = render::Object::readVerticesFromFile("../models/floor.obj");
        objects.push_back(renderer.createObject(std::move(b), std::move(j)));
        objects.at(1).transform.scale = {100.0f, 100.0f, 100.0f};
        objects.at(1).transform.translation.y -= 10.0f;

        render::Camera camera {{}, 0, 0};

        while (!renderer.shouldClose())
        {
            seb::logLog("FPS: {}", 1.0f / renderer.getDeltaTimeSeconds());

            camera.updateFromKeys(keyCallback, renderer.getDeltaTimeSeconds());

            renderer.drawFrame(camera, objects);
        }
    }
    catch (const std::exception& e)
    {
        seb::logFatal("Exception propagated to main! | {}", e.what());
    }
}