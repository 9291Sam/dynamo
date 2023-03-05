#ifndef SRC_RENDER_VULKAN_ALLOCATOR_HPP
#define SRC_RENDER_VULKAN_ALLOCATOR_HPP

#include "includes.hpp"

namespace render
{
    class Allocator
    {
    public:

        Allocator(
            vk::Instance,
            vk::PhysicalDevice,
            vk::Device,
            PFN_vkGetInstanceProcAddr,
            PFN_vkGetDeviceProcAddr);
        ~Allocator();

        Allocator(const Allocator&)            = delete;
        Allocator(Allocator&&)                 = default;
        Allocator& operator=(const Allocator&) = delete;
        Allocator& operator=(Allocator&&)      = default;

        [[nodiscard]] VmaAllocator operator*() const;

    private:
        VmaAllocator allocator;
    };

} // namespace render

#endif // SRC_RENDER_ALLOCATOR_HPP