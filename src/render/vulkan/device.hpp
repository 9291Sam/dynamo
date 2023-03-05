#ifndef SRC_RENDER_VULKAN_DEVICE_HPP
#define SRC_RENDER_VULKAN_DEVICE_HPP

#include "includes.hpp"

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

        [[nodiscard]] bool shouldBuffersStage() const;

        [[nodiscard]] vk::PhysicalDevice asPhysicalDevice() const;
        [[nodiscard]] vk::Device asLogicalDevice() const;

        [[nodiscard]] std::uint32_t getRenderIndex() const;
        [[nodiscard]] vk::Queue getRenderQueue() const;

        // [[nodiscard]] std::uint32_t getComputeIndex() const;
        // [[nodiscard]] vk::Queue getComputeQueue() const;

    private:
        vk::PhysicalDevice physical_device;
        vk::UniqueDevice   logical_device;
        std::uint32_t      render_index;
        vk::Queue          render_queue;
        // std::uint32_t      compute_index;
        // vk::Queue          compute_queue;
        bool               stage_buffers;
    }; // class Device
} // namespace render 

#endif // SRC_RENDER_DEVICE_HPP