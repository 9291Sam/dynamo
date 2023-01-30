#include "vulkan_includes.hpp"

#include <sebib/seblog.hpp>

#include "renderer.hpp"

namespace render
{
    Renderer::Renderer()
        : window {{1200, 1200}, "Dynamo"}
        , instance {nullptr}
        , draw_surface {nullptr}
        , device {nullptr}
        , allocator {nullptr}
    {
        const vk::DynamicLoader dl;
        const PFN_vkGetInstanceProcAddr dynVkGetInstanceProcAddr = 
            dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(dynVkGetInstanceProcAddr);

        this->instance = std::make_unique<Instance>(dynVkGetInstanceProcAddr);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(**this->instance);

        this->draw_surface = window.createSurface(**this->instance);

        this->device = std::make_unique<Device>(**this->instance, *this->draw_surface);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(**this->instance, this->device->asLogicalDevice());

        this->allocator = std::make_unique<Allocator>(
            **this->instance,
            this->device->asPhysicalDevice(),
            this->device->asLogicalDevice(),
            dynVkGetInstanceProcAddr,
            dl.getProcAddress<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr")
        );

        this->swapchain = std::make_unique<Swapchain>(
            *this->device,
            *this->draw_surface,
            vk::Extent2D { // TODO: once we get this from the window, ensure its within range by modifying the wondows code to make sure its right
                .width {1200},
                .height {1200},
            }
        );

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);

        seb::todo<>("");
    }

} // namespace render