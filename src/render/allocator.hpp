#pragma once

#ifndef SRC_RENDER_ALLOCATOR_HPP
#define SRC_RENDER_ALLOCATOR_HPP

#include "vulkan_includes.hpp"

namespace render
{
    class Allocator
    {
    public:

        Allocator(vk::Instance, vk::PhysicalDevice, vk::Device, PFN_vkGetInstanceProcAddr,
            PFN_vkGetDeviceProcAddr);
        ~Allocator();

        Allocator()                            = delete;
        Allocator(const Allocator&)            = delete;
        Allocator(Allocator&&)                 = delete;
        Allocator& operator=(const Allocator&) = delete;
        Allocator& operator=(Allocator&&)      = delete;

        [[nodiscard, gnu::pure]] VmaAllocator operator*() const;

    private:
        VmaAllocator allocator;
    };

} // namespace render

#endif // SRC_RENDER_ALLOCATOR_HPP