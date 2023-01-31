#pragma once

#ifndef SRC_RENDER_SWAPCHAIN_HPP
#define SRC_RENDER_SWAPCHAIN_HPP

#include "vulkan_includes.hpp"

#include "device.hpp"

namespace render
{
    class Swapchain
    {
    public:

        Swapchain(const Device&, vk::SurfaceKHR, vk::Extent2D);
        ~Swapchain()                           = default;

        Swapchain()                            = delete;
        Swapchain(const Swapchain&)            = delete;
        Swapchain(Swapchain&&)                 = delete;
        Swapchain& operator=(const Swapchain&) = delete;
        Swapchain& operator=(Swapchain&&)      = delete;

        [[nodiscard, gnu::pure]] vk::SurfaceFormatKHR getSurfaceFormat() const;

    private:
        vk::Extent2D           extent;
        vk::SurfaceFormatKHR   format;
        vk::UniqueSwapchainKHR swapchain;

        std::vector<vk::Image> images; /* TODO: replace with an image class*/
        std::vector<vk::UniqueImageView> image_views;
    }; // class Swapchain
} // namespace render

#endif // SRC_RENDER_SWAPCHAIN_HPP