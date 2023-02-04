#include <sebib/seblog.hpp>

#include "device.hpp"

std::uint32_t findIndexOfGraphicsAndPresentQueue(vk::PhysicalDevice pD, vk::SurfaceKHR surface)
{
    std::uint32_t idx = 0;
    for (auto q : pD.getQueueFamilyProperties())
    {
        if (!(q.queueFlags & vk::QueueFlagBits::eGraphics))
        {
            continue;
        }

        if (!pD.getSurfaceSupportKHR(idx, surface))
        {
            continue;
        }

        return idx;
    }

    seb::panic("Failed to find a suitable Graphics + Present queue");
}

std::size_t getDeviceRating(vk::PhysicalDevice device)
{
    std::size_t score = 0;

    const auto deviceLimits {device.getProperties().limits};

    score += deviceLimits.maxImageDimension2D; 
    score += deviceLimits.maxImageDimension3D;

    return score;
}

vk::PhysicalDevice findBestDevice(const std::vector<vk::PhysicalDevice>& devices)
{
    return *std::max_element(
        devices.begin(),
        devices.end(), 
        [](vk::PhysicalDevice dev1, vk::PhysicalDevice dev2)
        {
            return getDeviceRating(dev1) < getDeviceRating(dev2);
        }
    );
}

namespace render
{
    Device::Device(vk::Instance instance, vk::SurfaceKHR drawSurface)
    {
        this->physical_device = findBestDevice(instance.enumeratePhysicalDevices());
        
        this->queue_index = findIndexOfGraphicsAndPresentQueue(this->physical_device, drawSurface);
        const float One = 1.0f;
        const std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos
        {
            vk::DeviceQueueCreateInfo
            {
                .sType            {vk::StructureType::eDeviceQueueCreateInfo},
                .pNext            {nullptr},
                .flags            {},
                .queueFamilyIndex {this->queue_index},
                .queueCount       {1},
                .pQueuePriorities {&One},
            }
        };

        const std::vector<const char*> DeviceExtensions {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            #ifdef __APPLE__
                VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
            #endif // __APPLE__
        };

        const vk::PhysicalDeviceFeatures DeviceFeatures = {};

        const vk::DeviceCreateInfo deviceCreateInfo
        {
            .sType                   {vk::StructureType::eDeviceCreateInfo},
            .pNext                   {nullptr},
            .flags                   {},
            .queueCreateInfoCount    {static_cast<std::uint32_t>(queueCreateInfos.size())},
            .pQueueCreateInfos       {queueCreateInfos.data()},
            .enabledLayerCount       {0}, // These are depreciated
            .ppEnabledLayerNames     {nullptr},
            .enabledExtensionCount   {static_cast<std::uint32_t>(DeviceExtensions.size())},
            .ppEnabledExtensionNames {DeviceExtensions.data()},
            .pEnabledFeatures        {&DeviceFeatures},
        };

        this->logical_device = this->physical_device.createDeviceUnique(deviceCreateInfo);

        this->queue = this->logical_device->getQueue(this->queue_index, 0);

        

        this->stage_buffers = [this]
        {
            // auto memoryProperties = this->physical_device.getMemoryProperties();

            seb::logWarn("add staging buffer test");

            return true;
        }();
    }

    bool Device::shouldBuffersStage() const
    {
        return this->stage_buffers;
    }


    vk::PhysicalDevice Device::asPhysicalDevice() const
    {
        return this->physical_device;
    }

    vk::Device Device::asLogicalDevice() const
    {
        return *this->logical_device;
    }


    std::uint32_t Device::getRenderQueueIndex() const
    {
        return this->queue_index;
    }

    vk::Queue Device::getRenderQueue() const
    {
        return this->queue;
    }
} // namespace render

