#pragma once

#ifndef SRC_RENDER_SWAPCHAIN_HPP
#define SRC_RENDER_SWAPCHAIN_HPP

#include "vulkan_includes.hpp"

namespace render
{
    class Swapchain
    {
    public:

        Swapchain(vk::PhysicalDevice, vk::Device, vk::SurfaceKHR, vk::Extent2D);
        ~Swapchain();

        Swapchain()                            = delete;
        Swapchain(const Swapchain&)            = delete;
        Swapchain(Swapchain&&)                 = delete;
        Swapchain& operator=(const Swapchain&) = delete;
        Swapchain& operator=(Swapchain&&)      = delete; 

    private:
        vk::Extent2D           extent;
        vk::SurfaceFormatKHR   format;
        vk::UniqueSwapchainKHR swapchain;

        /* Image images */
    }; // class Swapchain
} // namespace render

#endif // SRC_RENDER_SWAPCHAIN_HPP