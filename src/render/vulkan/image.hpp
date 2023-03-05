#ifndef SRC_RENDER_VULKAN_IMAGE_HPP
#define SRC_RENDER_VULKAN_IMAGE_HPP

#include "includes.hpp"

#include "allocator.hpp"
#include "buffer.hpp"

namespace render
{
    class Image2D
    {
    public:

        Image2D(const Allocator&, vk::Device, vk::Extent2D, vk::Format, vk::ImageUsageFlags, 
            vk::ImageAspectFlags, vk::ImageTiling, vk::MemoryPropertyFlags);
        ~Image2D();

        Image2D()                          = delete;
        Image2D(const Image2D&)            = delete;
        Image2D(Image2D&&)                 = delete;
        Image2D& operator=(const Image2D&) = delete;
        Image2D& operator=(Image2D&&)      = delete;

        [[nodiscard]] vk::ImageView operator*() const;
        [[nodiscard]] vk::Format getFormat() const;
        [[nodiscard]] vk::ImageLayout getLayout() const;
        
        void transitionLayout(vk::CommandBuffer, vk::ImageLayout from, vk::ImageLayout to,
            vk::PipelineStageFlags sourceStage, vk::PipelineStageFlags destinationStage,
            vk::AccessFlags sourceAccess, vk::AccessFlags destinationAccess);
        void copyFromBuffer(vk::CommandBuffer, const Buffer&) const;

    private:
        vk::Image            image;
        vk::Extent2D         extent;
        vk::ImageAspectFlags aspect;
        vk::Format           format;
        vk::ImageLayout      layout;
        vk::UniqueImageView  view;
        VmaAllocation        memory;
        const Allocator*     maybeAllocator;
    };

} // namespace render

#endif // SRC_RENDER_VULKAN_IMAGE_HPP