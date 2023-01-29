#include <sebib/seblog.hpp>

#include "allocator.hpp"

namespace render
{
    Allocator::Allocator(vk::Instance instance, vk::PhysicalDevice physicalDevice,
        vk::Device logicalDevice, PFN_vkGetInstanceProcAddr dynVkGetInstanceProcAddr,
        PFN_vkGetDeviceProcAddr dynVkGetDeviceProcAddr)
    {
        // Allocator setup
        VmaVulkanFunctions vulkanFunctions {};
        vulkanFunctions.vkGetInstanceProcAddr = dynVkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr   = dynVkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo {};
        // allocatorCreateInfo.flags  = {}; /* TODO: threadsafety performance flag! */
        allocatorCreateInfo.physicalDevice              = {physicalDevice};
        allocatorCreateInfo.device                      = {logicalDevice};
        allocatorCreateInfo.pVulkanFunctions            = {&vulkanFunctions}; 
        allocatorCreateInfo.instance                    = {instance};
        allocatorCreateInfo.vulkanApiVersion            = {VK_API_VERSION_1_0};

        VkResult result = vmaCreateAllocator(&allocatorCreateInfo, &this->allocator);
        seb::assertFatal(
            result == VK_SUCCESS,
            "Failed to create vma allocator {}",
            vk::to_string(vk::Result {result})
        );
    }

    Allocator::~Allocator()
    {
        vmaDestroyAllocator(this->allocator);
    }
} // namespace render