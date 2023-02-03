#include <sebib/seblog.hpp>

#include "swapchain.hpp"

namespace render
{
    Swapchain::Swapchain(const Device& device, vk::SurfaceKHR surface, vk::Extent2D extent_)
        : extent {extent_} // TODO: do we need this extent?
    {
        const vk::SurfaceFormatKHR idealSurfaceFormat
        {
            .format {vk::Format::eB8G8R8A8Srgb},
            .colorSpace {vk::ColorSpaceKHR::eSrgbNonlinear}
        };
        const auto availableSurfaceFormats = 
            device.asPhysicalDevice().getSurfaceFormatsKHR(surface);
        if (std::find(
                availableSurfaceFormats.begin(),
                availableSurfaceFormats.end(),
                idealSurfaceFormat
            ) == availableSurfaceFormats.end())
        {
            // TODO: find closest
            seb::panic("Failed to find ideal surface format!");
        }
        this->format = idealSurfaceFormat;


        const vk::SurfaceCapabilitiesKHR surfaceCapabilities =
            device.asPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
        const vk::Extent2D minExtent = surfaceCapabilities.minImageExtent;
        const vk::Extent2D maxExtent = surfaceCapabilities.maxImageExtent;
        // seb::logTrace(
        //     "minExtent {} {} | maxExtent {} {} | reqExtent {} {}",
        //     minExtent.width, minExtent.height,
        //     maxExtent.width, maxExtent.height,
        //     this->extent.width, this->extent.height
        // );
        seb::assertFatal(minExtent.width  <= this->extent.width,  "Desired image extent width less than available)");
        seb::assertFatal(minExtent.height <= this->extent.height, "Desired image extent height less than available)");
        seb::assertFatal(maxExtent.width  >= this->extent.width,  "Desired image extent width greater than available");
        seb::assertFatal(maxExtent.height >= this->extent.height, "Desired image extent height greater than available");
        

        const std::vector<vk::PresentModeKHR> availablePresentModes =
            device.asPhysicalDevice().getSurfacePresentModesKHR(surface);
        const vk::PresentModeKHR selectedPresentMode = std::find(
                availablePresentModes.cbegin(), 
                availablePresentModes.cend(), 
                vk::PresentModeKHR::eMailbox
            ) != availablePresentModes.cend()
            ? vk::PresentModeKHR::eMailbox
            : vk::PresentModeKHR::eFifo;

        
        const std::array<std::uint32_t, 1> QueueFamilyIndicies {device.getRenderQueueIndex()};
        const vk::SwapchainCreateInfoKHR SwapchainCreateInfoKHR
        {
            .sType   {vk::StructureType::eSwapchainCreateInfoKHR},
            .pNext   {nullptr},
            .flags   {},
            .surface {surface},
            .minImageCount { // PERF: what
                surfaceCapabilities.maxImageCount == 0 
                ? surfaceCapabilities.minImageCount + 1
                : surfaceCapabilities.maxImageCount
            },
            .imageFormat           {this->format.format},
            .imageColorSpace       {this->format.colorSpace},
            .imageExtent           {this->extent},
            .imageArrayLayers      {1},
            .imageUsage            {vk::ImageUsageFlagBits::eColorAttachment},
            .imageSharingMode      {vk::SharingMode::eExclusive},
            .queueFamilyIndexCount {static_cast<std::uint32_t>(QueueFamilyIndicies.size())},
            .pQueueFamilyIndices   {QueueFamilyIndicies.data()},
            .preTransform          {surfaceCapabilities.currentTransform},
            .compositeAlpha        {vk::CompositeAlphaFlagBitsKHR::eOpaque},
            .presentMode           {selectedPresentMode},
            .clipped               {true},
            .oldSwapchain          {nullptr}
        };

        this->swapchain = device.asLogicalDevice().createSwapchainKHRUnique(SwapchainCreateInfoKHR);
        this->images = device.asLogicalDevice().getSwapchainImagesKHR(*this->swapchain);
        
        this->image_views = [this, &device]{
            
            std::vector<vk::UniqueImageView> output {};
            output.reserve(this->images.size());

            for (const vk::Image& i : this->images)
            {
                vk::ImageViewCreateInfo createInfo {
                    .sType            {vk::StructureType::eImageViewCreateInfo},
                    .pNext            {nullptr},
                    .flags            {},
                    .image            {i},
                    .viewType         {vk::ImageViewType::e2D},
                    .format           {this->format.format},
                    .components       {
                        .r {vk::ComponentSwizzle::eIdentity},
                        .g {vk::ComponentSwizzle::eIdentity},
                        .b {vk::ComponentSwizzle::eIdentity},
                        .a {vk::ComponentSwizzle::eIdentity},
                    },
                    .subresourceRange {
                        .aspectMask     {vk::ImageAspectFlagBits::eColor},
                        .baseMipLevel   {0},
                        .levelCount     {1},
                        .baseArrayLayer {0},
                        .layerCount     {1},
                    },
                };

                output.push_back(device.asLogicalDevice().createImageViewUnique(createInfo));
            }
            return output;
        }();
    }

    vk::SwapchainKHR Swapchain::operator*() const
    {
        return *this->swapchain;
    }

    vk::Extent2D Swapchain::getExtent() const
    {
        return this->extent;
    }

    vk::SurfaceFormatKHR Swapchain::getSurfaceFormat() const
    {
        return this->format;
    }

    auto Swapchain::getImageViews() const
        -> const std::vector<vk::UniqueImageView>&
    {
        return this->image_views;
    }
} // namespace render