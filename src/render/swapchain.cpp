#include "swapchain.hpp"

namespace render
{
    Swapchain::Swapchain(vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, 
        vk::SurfaceKHR surface, vk::Extent2D extent_)
        : extent {extent_}
    {
        const vk::SurfaceCapabilitiesKHR = physicalDevice.getSurfaceCapabilitiesKHR(surface);

        const vk::SwapchainCreateInfoKHR SwapchainCreateInfoKHR
        {
            .sTyoe {vk::StructureType::eSwapchainCreateInfoKHR},
            .pNext {nullptr},
            .flags {},
            .surface {surface},
        }


    }
} // namespace render