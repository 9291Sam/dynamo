#pragma once

#ifndef SRC_RENDER_DEVICE_HPP
#define SRC_RENDER_DEVICE_HPP

#include "vulkan_includes.hpp"

namespace render
{
    /// @brief Abstraction over a combined physical and logical device
    /// TODO: update to seperate render / present queues
    class Device
    {
    public:
    
        Device(vk::Instance, vk::SurfaceKHR);
        ~Device()                        = default;

        Device()                         = delete;
        Device(const Device&)            = delete;
        Device(Device&&)                 = delete;
        Device& operator=(const Device&) = delete;
        Device& operator=(Device&&)      = delete;

        [[nodiscard, gnu::pure]] bool shouldBuffersStage() const;

        [[nodiscard, gnu::pure]] vk::PhysicalDevice asPhysicalDevice() const;
        [[nodiscard, gnu::pure]] vk::Device asLogicalDevice() const;
        [[nodiscard, gnu::pure]] std::uint32_t getRenderQueueIndex() const;
        [[nodiscard, gnu::pure]] vk::Queue getRenderQueue() const;

    private:
        vk::PhysicalDevice physical_device;
        vk::UniqueDevice   logical_device;
        std::uint32_t      queue_index;
        vk::Queue          queue;
        bool               stage_buffers;
    }; // class Device
} // namespace render 

#endif // SRC_RENDER_DEVICE_HPP