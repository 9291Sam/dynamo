#include <iostream>

#include <sebib/seblog.hpp>
#include <render/renderer.hpp>
#include <world/world.hpp>

#include <memory>
#include <optional>

template<class T>
const T& max(const T& t1, const T& t2)
{
    if (t1 < t2)
    {
        return t2;
    }
    else
    {
        return t1;
    }
}

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

        renderer.setCursorMode(vkfw::CursorMode::eDisabled);

        while (!renderer.shouldClose())
        {
            if (renderer.getKeyCallback()(vkfw::Key::eJ))
            {
                seb::logLog("FPS: {} | Camera: {}", 
                    1.0f / renderer.getDeltaTimeSeconds(), 
                    static_cast<std::string>(camera)
                );
            }

            // Need two abstracted modes.
            // Cursor infinite and disabled
            // Clickable cursor on the screen (no camera change!)

            if (renderer.getKeyCallback()(vkfw::Key::eT))
            {
                renderer.setCursorMode(vkfw::CursorMode::eNormal);
            }

            if (renderer.getKeyCallback()(vkfw::Key::eY))
            {
                renderer.setCursorMode(vkfw::CursorMode::eDisabled);
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