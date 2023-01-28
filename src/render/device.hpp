#pragma once

#ifndef SRC_RENDER_DEVICE_HPP
#define SRC_RENDER_DEVICE_HPP

#include "vulkan_includes.hpp"

namespace render
{
    /// @brief Abstraction over a combined physical and logical device
    class Device
    {
    public:
        Device(vk::Instance, vk::SurfaceKHR);
        ~Device();

        Device(const Device&)            = delete;
        Device(Device&&)                 = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&)      = delete;

        [[nodiscard]] vk::PhysicalDevice asPhysicalDevice() const;
        [[nodiscard]] vk::Device asLogicalDevice() const;
        [[nodiscard]] VmaAllocator getAllocator() const;
        [[nodiscard]] vk::Queue getRenderQueue() const;

    private:
        vk::PhysicalDevice physical_device;
        vk::UniqueDevice logical_device;
        VmaAllocator allocator;
        vk::Queue queue;
    }; // class Device
} // namespace render 

#endif // SRC_RENDER_DEVICE_HPP