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
        Device();
        ~Device();

        Device(const Device&)            = delete;
        Device(Device&&)                 = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&)      = delete;

        [[nodiscard]] vk::PhysicalDevice asPhysicalDevice() const;
        [[nodiscard]] vk::device asLogicalDevice() const;
        [[nodiscard]] vma::Allocator getAllocator() const;
        [[nodiscard]] vk::Queue getQueue() const;

    private:
        vk::physicalDevice physical_device;
        vk::UniqueDevice logical_device;
        vma::UniqueAllocator allocator;
        vk::UniqueQueue queue;
    }
}