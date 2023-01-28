#include <iostream>


#include <sebib/seblog.hpp>
#include <render/renderer.hpp>

int main()
{
    vk::DynamicLoader dl;
    PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    
    

    auto availablePhysicalDevices = instance->enumeratePhysicalDevices();
    seb::assertFatal(availablePhysicalDevices.size() > 0, "No vulkan devices are available");
    seb::logWarn("Picking first device TODO: select best gpu");
    auto physicalDevice =  availablePhysicalDevices.at(0);

    const float One = 1.0f;
    std::vector<vk::DeviceQueueCreateInfo>
    {
        vk::DeviceQueueCreateInfo
        {
            .sType            {vk::StructureType::eDeviceQueueCreateInfo},
            .pNext            {nullptr},
            .flags            {},
            .queueFamilyIndex {
                [](vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface){ // finds index of a graphics + present queue

                    std::uint32_t idx = 0;
                    for (auto q : physicalDevice.getQueueFamilyProperties())
                    {
                        if (!(q.queueFlags & vk::QueueFlagBits::eGraphics))
                        {
                            continue;
                        }

                        if (!physicalDevice.getSurfaceSupportKHR(idx, surface))
                        {
                            continue;
                        }

                        return idx;
                    }

                    seb::panic("Failed to find a suitable Graphics + Present queue");
                }(physicalDevice, surface)
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
        .queueCreateInfoCount    {},
        .pQueueCreateInfos       {},
        .enabledLayerCount       {},
        .ppEnabledLayerNames     {},
        .enabledExtensionCount   {},
        .ppEnabledExtensionNames {},
        .pEnabledFeatures        {},
    };

    //.createDeviceUnique()
    // vk::Device device = physicalDevices[0].createDevice({}, nullptr);
    // VULKAN_HPP_DEFAULT_DISPATCHER.init(device);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance, *device);

    seb::logLog("Hello World");
}