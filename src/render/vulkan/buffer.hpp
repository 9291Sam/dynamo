#ifndef SRC_RENDER_VULKAN_BUFFER_HPP
#define SRC_RENDER_VULKAN_BUFFER_HPP

#include <optional>
#include <span>

#include "device.hpp"

#include "includes.hpp"

namespace render
{
    class Buffer
    {
    public:

        Buffer(
            VmaAllocator,
            std::size_t sizeBytes,
            vk::BufferUsageFlags,
            vk::MemoryPropertyFlags);
        ~Buffer();

        Buffer(const Buffer&)            = delete;
        Buffer(Buffer&&);
        Buffer& operator=(const Buffer&) = delete;
        Buffer& operator=(Buffer&&);

        [[nodiscard]] vk::Buffer operator*() const;
        [[nodiscard]] std::size_t sizeBytes() const;
        [[nodiscard]] void* getMappedPtr() const;

        void write(std::span<const std::byte>) const;
        void copyFrom(const Buffer&, vk::CommandBuffer) const; 

    private:
        VmaAllocator         allocator;
        VkBuffer             buffer;
        VmaAllocation        allocation;
        vk::BufferUsageFlags usage;
        std::size_t          size_bytes;
        mutable void*        mapped_ptr;
    };

    class StagedBuffer
    {
    public:

        StagedBuffer(
            const Device&,
            VmaAllocator,
            std::size_t sizeBytes,
            vk::BufferUsageFlags);
        ~StagedBuffer()                              = default;

        StagedBuffer()                               = delete;
        StagedBuffer(const StagedBuffer&)            = delete;
        StagedBuffer(StagedBuffer&&)                 = default;
        StagedBuffer& operator=(const StagedBuffer&) = delete;
        StagedBuffer& operator=(StagedBuffer&&)      = default;
        
        void write(std::span<const std::byte>) const;
        void stage(vk::CommandBuffer) const;

    private:
        std::optional<Buffer> staging_buffer;
        Buffer                gpu_local_buffer;
        std::size_t           size_bytes;
    };
} // namespace render

#endif // SRC_RENDER_VULKAN_BUFFER_HPP