#include "vulkan_includes.hpp"

#include "renderer.hpp"

namespace render
{
    Renderer::Renderer()
        : window {"Dynamo", {1200, 1200}}
        , instance {nullptr}
        , draw_surface {nullptr}
        , device {nullptr}
    {
        vk::DynamicLoader dl;
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        this->instance = std::make_unique<Instance>();

        this->draw_surface = window.createSurface(*this->instance);

        this->device = std::make_unique<Device>(*this->instance, *this->draw_surface);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(**this->instance, this->device->asLogicalDevice());

    }

} // namespace render