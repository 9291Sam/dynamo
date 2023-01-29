#pragma once

#ifndef SRC_RENDER_ALLOCATOR_HPP
#define SRC_RENDER_ALLOCATOR_HPP

#include "vulkan_includes.hpp"

namespace render
{
    class Allocation;

    class Allocator
    {
    public:

        Allocator(vk::Instance, vk::PhysicalDevice, vk::Device, PFN_vkGetInstanceProcAddr,
            PFN_vkGetDeviceProcAddr);
        ~Allocator();

        Allocator(const Allocator&)            = delete;
        Allocator(Allocator&&)                 = delete;
        Allocator& operator=(const Allocator&) = delete;
        Allocator& operator=(Allocator&&)      = delete;

    private:
        friend class Allocation;

        VmaAllocator allocator;
    };

    class Allocation
    {
    public:

        ~Allocation();

        Allocator(const Allocator&)            = delete;
        Allocator(Allocator&&);
        Allocator& operator=(const Allocator&) = delete;
        Allocator& operator=(Allocator&&);

    private:
        
        Allocation(vk::Device, std::size_t sizeInBytes, vk::MemoryPropertyFlags);


    };

} // namespace render

#endif // SRC_RENDER_ALLOCATOR_HPP