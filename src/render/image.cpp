#include <sebib/seblog.hpp>

#include "image.hpp"

namespace render
{
    Image2D::Image2D(const Allocator& allocator_, vk::Device device, vk::Extent2D extent, 
        vk::Format format_, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
        vk::ImageTiling tiling, vk::MemoryPropertyFlags memoryFlags)
        : image          {nullptr}
        , format         {format_}
        , view           {nullptr}
        , memory         {nullptr}
        , maybeAllocator {&allocator_}
    {
        VkImageCreateInfo imageCreateInfo
        {
            .sType       {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO},
            .pNext       {nullptr},
            .flags       {},
            .imageType   {VK_IMAGE_TYPE_2D},
            .format      {static_cast<VkFormat>(format)},
            .extent      {
                VkExtent3D
                {
                    .width  {extent.width},
                    .height {extent.height},
                    .depth  {1}
                }
            },
            .mipLevels   {1},
            .arrayLayers {1},
            .samples     {VK_SAMPLE_COUNT_1_BIT},
            .tiling      {static_cast<VkImageTiling>(tiling)},
            .usage       {static_cast<VkImageUsageFlags>(usage)},
            .sharingMode {VK_SHARING_MODE_EXCLUSIVE},
            .queueFamilyIndexCount {0},
            .pQueueFamilyIndices   {nullptr},
            .initialLayout         {VK_IMAGE_LAYOUT_UNDEFINED}
        };

        VmaAllocationCreateInfo imageAllocationInfo
        {
            .flags          {},
            .usage          {VMA_MEMORY_USAGE_AUTO},
            .requiredFlags  {static_cast<VkMemoryPropertyFlags>(memoryFlags)},
            .preferredFlags {},
            .memoryTypeBits {},
            .pool           {nullptr},
            .pUserData      {nullptr},
            .priority       {1.0f}
        };

        VkImage vmaOutputImage = nullptr;
        seb::assertFatal(
            vmaCreateImage(
                **this->maybeAllocator,
                &imageCreateInfo,
                &imageAllocationInfo,
                &vmaOutputImage,
                &this->memory,
                nullptr
            ) == VK_SUCCESS,
            "Failed to create Image"
        );
        this->image = vk::Image {vmaOutputImage};



        vk::ImageViewCreateInfo imageViewCreateInfo
        {
            .sType            {vk::StructureType::eImageViewCreateInfo},
            .pNext            {nullptr},
            .flags            {},
            .image            {this->image},
            .viewType         {vk::ImageViewType::e2D},
            .format           {this->format},
            .components       {},
            .subresourceRange {
                vk::ImageSubresourceRange
                {
                    .aspectMask     {aspect},
                    .baseMipLevel   {0},
                    .levelCount     {1},
                    .baseArrayLayer {0},
                    .layerCount     {1},
                }
            },
        };

        this->view = device.createImageViewUnique(imageViewCreateInfo);
    }

    Image2D::~Image2D()
    {
        if (maybeAllocator != nullptr)
        {
            vmaDestroyImage(**this->maybeAllocator, static_cast<VkImage>(this->image), this->memory);
        }
    }

    // Image2D::Image2D(Image2D&& other)
    // {
    //     this->image          = other.image;
    //     this->format         = other.format;
    //     this->view           = std::move(other.view);
    //     this->memory         = other.memory;
    //     this->maybeAllocator = other.maybeAllocator;

    //     other.image = nullptr;
    //     other.memory = nullptr;
    //     other.maybeAllocator = nullptr;
    // }

    vk::ImageView Image2D::operator*() const
    {
        return *this->view;
    }

    vk::Format Image2D::getFormat() const
    {
        return this->format;
    }
    
} // namespace render