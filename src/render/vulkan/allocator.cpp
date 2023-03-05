#include <sebib/seblog.hpp>

#include "allocator.hpp"

namespace render
{
    Allocator::Allocator(
        vk::Instance instance, 
        vk::PhysicalDevice physicalDevice,
        vk::Device logicalDevice,
        PFN_vkGetInstanceProcAddr dynVkGetInstanceProcAddr,
        PFN_vkGetDeviceProcAddr dynVkGetDeviceProcAddr)
    {
        VmaVulkanFunctions vulkanFunctions {};
        vulkanFunctions.vkGetInstanceProcAddr = dynVkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr   = dynVkGetDeviceProcAddr;

        const VmaAllocatorCreateInfo allocatorCreateInfo
        {
            .flags                          {},
            .physicalDevice                 {physicalDevice},
            .device                         {logicalDevice},
            .preferredLargeHeapBlockSize    {0}, // chosen by VMA
            .pAllocationCallbacks           {nullptr},
            .pDeviceMemoryCallbacks         {nullptr},
            .pHeapSizeLimit                 {nullptr},
            .pVulkanFunctions               {&vulkanFunctions},
            .instance                       {instance},
            .vulkanApiVersion               {VK_API_VERSION_1_2},
            .pTypeExternalMemoryHandleTypes {nullptr},
        };

        const VkResult result = vmaCreateAllocator(
            &allocatorCreateInfo,
            &this->allocator
        );
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

    VmaAllocator Allocator::operator*() const
    {
        return this->allocator;
    }
} // namespace render