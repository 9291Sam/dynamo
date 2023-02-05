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
        world::World world {};
        
        std::unordered_map<world::Object, std::weak_ptr<render::Object>> objectMap;

        

        render::Camera camera {{}, 0, 0};

        while (!renderer.shouldClose())
        {
            seb::logLog("FPS: {}", 1.0f / renderer.getDeltaTimeSeconds());

            camera.updateFromKeys(renderer.getKeyCallback(), renderer.getDeltaTimeSeconds());

            world.tick();

            objectMap.addIfNotAlreadyIn();

            for (auto [worldObject, renderObject] : objectMap)
            {
                if (world.contains(worldObject))
                {
                    // do nothing
                }
                else
                {
                    // free object
                }
            }

            renderer.drawFrame(camera, objects);
        }
    }
    catch (const std::exception& e)
    {
        seb::logFatal("Exception propagated to main! | {}", e.what());
    }
}