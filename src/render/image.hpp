#pragma once

#ifndef SRC_RENDER_IMAGE_HPP
#define SRC_RENDER_IMAGE_HPP

#include "vulkan_includes.hpp"

#include "allocator.hpp"

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
        Image2D(Image2D&&);
        Image2D& operator=(const Image2D&) = delete;
        Image2D& operator=(Image2D&&);

        [[nodiscard, gnu::pure]] vk::ImageView operator*() const;
        [[nodiscard, gnu::pure]] vk::Format getFormat() const;

    private:
        vk::Image           image;
        vk::Format          format;
        vk::UniqueImageView view;
        VmaAllocation       memory;
        const Allocator*    maybeAllocator;
    };

} // namespace render

#endif // SRC_RENDER_IMAGE_HPP