#include <iostream>

#include <sebib/seblog.hpp>
#include <render/renderer.hpp>
#include <world/world.hpp>

#include <memory>
#include <optional>

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
        world::World world {renderer};        

        render::Camera camera {{-35.0f, 35.0f, 35.0f}, -0.570792479f, 0.785398f};

        while (!renderer.shouldClose())
        {
            if (renderer.getKeyCallback()(vkfw::Key::eJ))
            {
                seb::logLog("FPS: {} | Camera: {}", 
                    1.0f / renderer.getDeltaTimeSeconds(), 
                    static_cast<std::string>(camera)
                );
            }

            camera.update(renderer.getKeyCallback(), renderer.getMouseDelta(), renderer.getDeltaTimeSeconds());

            renderer.drawFrame(camera, world.getObjects());
        }
    }
    catch (const std::exception& e)
    {
        seb::logFatal("Exception propagated to main! | {}", e.what());
    }
}