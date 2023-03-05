#include <sebib/seblog.hpp>

#include "buffer.hpp"

namespace render
{
    Buffer::Buffer(
        VmaAllocator allocator_, 
        std::size_t sizeBytes_, 
        vk::BufferUsageFlags usage_,
        vk::MemoryPropertyFlags memoryProperty
    )
        : allocator  {allocator_}
        , buffer     {nullptr}
        , allocation {nullptr}
        , usage      {usage_}
        , size_bytes {sizeBytes_}
        , mapped_ptr {nullptr}
    {
        const VkBufferCreateInfo bufferCreateInfo
        {
            .sType                 {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO},
            .pNext                 {nullptr},
            .flags                 {},
            .size                  {this->size_bytes},
            .usage                 {static_cast<VkBufferUsageFlags>(this->usage)},
            .sharingMode           {VK_SHARING_MODE_EXCLUSIVE},
            .queueFamilyIndexCount {0},
            .pQueueFamilyIndices   {nullptr},
        };

        const VmaAllocationCreateInfo allocationCreateInfo
        {
            .flags          {VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT},
            .usage          {VMA_MEMORY_USAGE_AUTO},
            .requiredFlags  {static_cast<VkMemoryPropertyFlags>(memoryProperty)},
            .preferredFlags {},
            .memoryTypeBits {},
            .pool           {nullptr},
            .pUserData      {},
            .priority       {}
        };

        seb::assertFatal(
            vmaCreateBuffer(
                allocator,
                &bufferCreateInfo,
                &allocationCreateInfo,
                &this->buffer, 
                &this->allocation,
                nullptr
            ) == VK_SUCCESS,
            "Failed to allocate buffer"
        );
    }
    
    Buffer::~Buffer()
    {
        if (this->mapped_ptr)
        {
            vmaUnmapMemory(this->allocator, this->allocation);
        }
        vmaDestroyBuffer(this->allocator, this->buffer, this->allocation);
    }

    Buffer::Buffer(Buffer&& other)
        : allocator  {other.allocator}
        , buffer     {other.buffer}
        , allocation {other.allocation}
        , usage      {other.usage}
        , size_bytes {other.size_bytes}
        , mapped_ptr {other.mapped_ptr}
    {
        other.buffer     = nullptr;
        other.allocation = nullptr;
        other.size_bytes = 0;
        other.mapped_ptr = nullptr;
    }

    Buffer& Buffer::operator=(Buffer&& other)
    {
        seb::assertFatal(
            this->allocator == other.allocator,
            "Allocators were not the same"
        );
        
        this->buffer     = other.buffer;
        this->allocation = other.allocation;
        this->usage      = other.usage;
        this->size_bytes = other.size_bytes;
        this->mapped_ptr = other.mapped_ptr;

        other.buffer     = nullptr;
        other.allocation = nullptr;
        other.size_bytes = 0;
        other.mapped_ptr = nullptr;

        return *this;
    }

    vk::Buffer Buffer::operator*() const
    {
        return vk::Buffer {this->buffer};
    }

    std::size_t Buffer::sizeBytes() const
    {
        return this->size_bytes;
    }

    void* Buffer::get_mapped_ptr() const
    {
        if (!this->mapped_ptr)
        {
            seb::assertFatal(
                vmaMapMemory(
                    this->allocator, 
                    this->allocation,
                    &this->mapped_ptr
                ) == VK_SUCCESS,
                "Failed to map buffer memory"
            );
            seb::assertFatal(
                this->mapped_ptr != nullptr,
                "Tried to map a nullptr"
            );
        }

        return this->mapped_ptr;
    }

    void Buffer::write(std::span<const std::byte> byteSpan) const
    {
        seb::assertFatal(
            byteSpan.size_bytes() == this->size_bytes,
            "Tried to write {} Bytes of data to a buffer of size {}",
            byteSpan.size_bytes(),
            this->size_bytes
        );

        void* mappedPtr = nullptr;
        seb::assertFatal(
            vmaMapMemory(
                this->allocator, 
                this->allocation,
                &mappedPtr
            ) == VK_SUCCESS, "Failed to map buffer memory"
        );
        seb::assertFatal(mappedPtr != nullptr, "Tried to map a nullptr");

        std::memcpy(mappedPtr, byteSpan.data(), byteSpan.size_bytes());

        vmaUnmapMemory(this->allocator, this->allocation);
    }

    void Buffer::copyFrom(const Buffer& other, vk::CommandBuffer commandBuffer) const
    {
        seb::assertFatal(
            this->size_bytes == other.size_bytes,
            "Tried to write a buffer of size {} to a buffer of size {}",
            other.size_bytes, 
            this->size_bytes
        );

        seb::assertFatal(this->allocator == other.allocator, "Allocators were not the same");

        vk::BufferCopy bufferCopyParameters 
        {
            .srcOffset {0},
            .dstOffset {0},
            .size      {this->size_bytes},
        };

        // SRC -> DST
        commandBuffer.copyBuffer(other.buffer, this->buffer, bufferCopyParameters);
    }

    StagedBuffer::StagedBuffer(const Device& device, VmaAllocator allocator, 
        std::size_t sizeBytes, vk::BufferUsageFlags usage)
        : staging_buffer {
            device.shouldBuffersStage() 
            ?
            std::make_optional<Buffer>(Buffer {
                allocator, 
                sizeBytes, 
                vk::BufferUsageFlagBits::eTransferSrc, 
                vk::MemoryPropertyFlagBits::eHostVisible | 
                vk::MemoryPropertyFlagBits::eHostCoherent}
            )
            :
            std::nullopt
        }
        , gpu_local_buffer {
            allocator,
            sizeBytes, 
            vk::BufferUsageFlagBits::eTransferDst | usage,
            vk::MemoryPropertyFlagBits::eDeviceLocal
        }
        , size_bytes {sizeBytes}
    {}

    void StagedBuffer::write(std::span<const std::byte> data) const
    {
        if (this->staging_buffer.has_value())
        {
            this->staging_buffer->write(data);
        }
        else
        {
            this->gpu_local_buffer.write(data);
        }
    }

    void StagedBuffer::stage(vk::CommandBuffer commandBuffer) const
    {
        if (this->staging_buffer.has_value())
        {
            this->gpu_local_buffer.copyFrom(*this->staging_buffer, commandBuffer);
        }
    }
}