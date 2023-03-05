#ifndef SRC_RENDER_VULKAN_SWAPCHAIN_HPP
#define SRC_RENDER_VULKAN_SWAPCHAIN_HPP

#include "includes.hpp"

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

        [[nodiscard]] vk::SwapchainKHR operator*() const;

        [[nodiscard]] vk::Extent2D getExtent() const;

        [[nodiscard]] vk::SurfaceFormatKHR getSurfaceFormat() const;

        [[nodiscard]] auto getImageViews() const
            -> const std::vector<vk::UniqueImageView>&;

    private:
        vk::Extent2D           extent;
        vk::SurfaceFormatKHR   format;
        vk::UniqueSwapchainKHR swapchain;

        std::vector<vk::Image> images;
        std::vector<vk::UniqueImageView> image_views;
    }; // class Swapchain
} // namespace render

#endif // SRC_RENDER_SWAPCHAIN_HPP