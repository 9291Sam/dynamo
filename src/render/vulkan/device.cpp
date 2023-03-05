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

        if (!(q.queueFlags & vk::QueueFlagBits::eTransfer))
        {
            continue;
        }

        if (!pD.getSurfaceSupportKHR(idx, surface))
        {
            continue;
        }

        return idx;
    }

    seb::panic("Failed to find a suitable queue");
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
        
        this->render_index = findIndexOfGraphicsAndPresentQueue(this->physical_device, drawSurface);
        const float One = 1.0f;
        const std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos
        {
            vk::DeviceQueueCreateInfo
            {
                .sType            {vk::StructureType::eDeviceQueueCreateInfo},
                .pNext            {nullptr},
                .flags            {},
                .queueFamilyIndex {this->render_index},
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

        vk::PhysicalDeviceFeatures deviceFeatures = {};
        deviceFeatures.samplerAnisotropy = true;

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
            .pEnabledFeatures        {&deviceFeatures},
        };

        this->logical_device = this->physical_device.createDeviceUnique(deviceCreateInfo);

        this->render_queue = this->logical_device->getQueue(this->render_index, 0);

        this->stage_buffers = [this]
        {
            auto memoryProperties = this->physical_device.getMemoryProperties();

            std::vector<vk::MemoryType> memoryTypes;
            std::vector<vk::MemoryHeap> memoryHeaps;

            std::optional<std::size_t> idx_of_gpu_main_memory = std::nullopt;
            
            for (std::size_t i = 0; i < memoryProperties.memoryTypeCount; i++)
            {
                memoryTypes.push_back(memoryProperties.memoryTypes.at(i));
            }

            for (std::size_t i = 0; i < memoryProperties.memoryHeapCount; i++)
            {
                memoryHeaps.push_back(memoryProperties.memoryHeaps.at(i));
            }

            const vk::MemoryPropertyFlags desiredFlags = 
                vk::MemoryPropertyFlagBits::eDeviceLocal |
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent;

            for (auto t : memoryTypes)
            {
                if ((t.propertyFlags & desiredFlags) == desiredFlags)
                {
                    seb::assertWarn(
                        !idx_of_gpu_main_memory.has_value(),
                        "There should only be one memory pool with `desiredFlags`!"
                    );

                    idx_of_gpu_main_memory = t.heapIndex;
                }
            }

            if (idx_of_gpu_main_memory.has_value())
            {
                if (memoryProperties.memoryHeaps.at(idx_of_gpu_main_memory.value()).size > 257 * 1024 * 1024)
                {
                    return false;
                }
            }
            return true;
        }();

        if (!this->stage_buffers)
        {
            seb::logLog("Resizable BAR support detected");
        }
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


    std::uint32_t Device::getRenderComputeTransferIndex() const
    {
        return this->render_index;
    }

    vk::Queue Device::getRenderComputeTransferQueue() const
    {
        return this->render_queue;
    }
} // namespace render

