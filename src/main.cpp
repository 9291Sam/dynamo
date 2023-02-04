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
        
        Window window {{1200, 1200}, "Sebgen-vkhp"};
        render::Renderer renderer {window};

        std::vector<render::Object> objects;

        auto [v, i] = render::Object::readVerticesFromFile("../models/gizmo.obj");
        objects.push_back(renderer.createObject(std::move(v), std::move(i)));
        objects.at(0).transform.scale = {4.0f, 4.0f, 4.0f};

        auto [b, j] = render::Object::readVerticesFromFile("../models/floor.obj");
        objects.push_back(renderer.createObject(std::move(b), std::move(j)));
        objects.at(1).transform.scale = {100.0f, 100.0f, 100.0f};
        objects.at(1).transform.translation.y -= 10.0f;

        render::Camera camera {{}, 0, 0};

        while (!window.shouldClose())
        {
            auto keyCallback = [&window](vkfw::Key key) -> bool
            {
                return window.isKeyPressed(key);
            };

            window.pollEvents();

            camera.updateFromKeys(keyCallback, window.getFrameTimeS());

            if (!renderer.drawFrame(camera, objects))
            {
                renderer.resize(window);
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}