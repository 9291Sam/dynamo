#include <iostream>

#include <sebib/seblog.hpp>
#include <render/renderer.hpp>
#include <world/world.hpp>

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

        render::Camera camera {{}, 0, 0};

        while (!renderer.shouldClose())
        {
            if (renderer.getKeyCallback()(vkfw::Key::eJ))
            {
                seb::logLog("FPS: {} | Camera: {:.52}", 
                    1.0f / renderer.getDeltaTimeSeconds(), 
                    static_cast<std::string>(camera)
                );
            }

            camera.updateFromKeys(renderer.getKeyCallback(), renderer.getDeltaTimeSeconds());

            // world.tick();

            // objectMap.addIfNotAlreadyIn();

            // for (auto [worldObject, renderObject] : objectMap)
            // {
            //     if (world.contains(worldObject))
            //     {
            //         // do nothing
            //     }
            //     else
            //     {
            //         // free object
            //     }
            // }

            renderer.drawFrame(camera, world.getObjects());
        }
    }
    catch (const std::exception& e)
    {
        seb::logFatal("Exception propagated to main! | {}", e.what());
    }
}