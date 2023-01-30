#include "swapchain.hpp"

namespace render
{
    Swapchain::Swapchain(vk::PhysicalDevice physicalDevice, vk::Device logicalDevice, 
        vk::SurfaceKHR surface, vk::Extent2D extent_)
        : extent {extent_} // TODO: do we need this extent?
    {
        const vk::SurfaceFormatKHR idealSurfaceFormat
        {
            .format {vk::Format::eB8G8R8A8Srgb},
            .colorSpace {vk::ColorSpaceKHR::eSrgbNonlinear}
        };
        const std::vector<vk::Format> availableSurfaceFormats = 
            physicalDevice.getSurfaceFormatsKHR(surface);
        if (std::find(
                availableSurfaceFormats.begin(),
                availableSurfaceFormats.end(),
                idealSurfaceFormat
            ) == availableSurfaceFormats.end())
        {
            // TODO: find closest
            seb::panic("Failed to find ideal surface format!");
        }
        this->format = idealSurfaceFormat


        const vk::SurfaceCapabilitiesKHR surfaceCapabilities =
            physicalDevice.getSurfaceCapabilitiesKHR(surface);


        const std::vector<vk::PresentModeKHR> availablePresentModes =
            physicalDevice.getSurfacePresentModesKHR(surface);
        const selectedPresentMode = std::find(
                availablePresentModes.cbegin(), 
                availablePresentModes.cend(), 
                vk::PresentModeKHR::eMailbox
            ) != availablePresentModes.cend()
            ? vk::PresentModeKHR::eMailbox
            : vk::PresentModeKHR::eFifo

        

        const vk::SwapchainCreateInfoKHR SwapchainCreateInfoKHR
        {
            .sTyoe   {vk::StructureType::eSwapchainCreateInfoKHR},
            .pNext   {nullptr},
            .flags   {},
            .surface {surface},
            .minImageCount { // PERF: what
                surfaceCapabilities.maxImageCount == 0 
                ? surfaceCapabilities.minImageCount + 1
                : surfaceCapabilities.maxImageCount
            },
            .format          {this->format.format},
            .imageColorSpace {this->format.colorSpace},
            .imageExtent     {[&]{
                // weird macOS case where the display manager wants a different size
                if (surfaceCapabilities.currentExtent.width == 
                    std::numeric_limits<std::uint32_t>::max()
                    &&
                    surfaceCapabilities.currentExtent.height ==
                    std::numeric_limits<std::uint32_t>::max())
                {
                    auto [unclampedWidth, unclampedHeight] = window.getFrameBufferSize();

                    return vk::Extent2D {
                        std::clamp(
                            unclampedWidth,
                            swapchainSupport.capabilities.minImageExtent.width,
                            swapchainSupport.capabilities.maxImageExtent.width
                        ),
                        std::clamp(
                            unclampedHeight,
                            swapchainSupport.capabilities.minImageExtent.height,
                            swapchainSupport.capabilities.maxImageExtent.height
                        )
                    };
                }
                else
                {
                    return surfaceCapabilities.currentExtent;
                }
            }},
            .imageSharingMode {vk::SharingMode::eExclusive},
            .queueFamilyIndexCount {// TODO: pass in index
        }


    }
} // namespace render