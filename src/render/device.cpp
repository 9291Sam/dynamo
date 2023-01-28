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

namespace render
{
    Device::Device(vk::Instance instance, vk::SurfaceKHR drawSurface)
    {
        seb::logWarn("Picking first device TODO: select best gpu");
        this->physical_device = instance.enumeratePhysicalDevices().at(0);

        const float One = 1.0f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos
        {
            vk::DeviceQueueCreateInfo
            {
                .sType            {vk::StructureType::eDeviceQueueCreateInfo},
                .pNext            {nullptr},
                .flags            {},
                .queueFamilyIndex {
                    findIndexOfGraphicsAndPresentQueue(this->physical_device, drawSurface)
                },
                .queueCount       {1},
                .pQueuePriorities {&One},
            }
        };

        const vk::DeviceCreateInfo deviceCreateInfo
        {
            .sType                   {vk::StructureType::eDeviceCreateInfo},
            .pNext                   {nullptr},
            .flags                   {},
            .queueCreateInfoCount    {static_cast<std::uint32_t>(queueCreateInfos.size())},
            .pQueueCreateInfos       {queueCreateInfos.data()},
            .enabledLayerCount       {0}, // These are depreciated
            .ppEnabledLayerNames     {nullptr},
            .enabledExtensionCount   {},
            .ppEnabledExtensionNames {},
            .pEnabledFeatures        {},
        };

        seb::todo("Allocator and queues");

        //.createDeviceUnique()
        // vk::Device device = physicalDevices[0].createDevice({}, nullptr);
    }

    Device::~Device()
    {
        seb::todo("not fini");
    }

    [[nodiscard]] vk::PhysicalDevice Device::asPhysicalDevice() const
    {
        return this->physical_device;
    }

    [[nodiscard]] vk::Device Device::asLogicalDevice() const
    {
        return *this->logical_device;
    }

    [[nodiscard]] VmaAllocator Device::getAllocator() const
    {
        return this->allocator;
    }

    [[nodiscard]] vk::Queue Device::getRenderQueue() const
    {
        return this->queue;
    }
} // namespace render

