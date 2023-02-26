#include <sebib/seblog.hpp>

#include "image.hpp"

namespace render
{
    Image2D::Image2D(const Allocator& allocator_, vk::Device device, vk::Extent2D extent_, 
        vk::Format format_, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect_,
        vk::ImageTiling tiling, vk::MemoryPropertyFlags memoryFlags)
        : image          {nullptr}
        , extent         {extent_}
        , aspect         {aspect_}
        , format         {format_}
        , layout         {vk::ImageLayout::eUndefined}
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
                    .aspectMask     {this->aspect},
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

    vk::ImageView Image2D::operator*() const
    {
        return *this->view;
    }

    vk::Format Image2D::getFormat() const
    {
        return this->format;
    }

    void Image2D::transitionLayout(vk::CommandBuffer commandBuffer,
        vk::ImageLayout from, vk::ImageLayout to,
        vk::PipelineStageFlags sourceStage, vk::PipelineStageFlags destinationStage,
        vk::AccessFlags sourceAccess, vk::AccessFlags destinationAccess)
    {
        seb::assertFatal(this->layout == from, 
            "Incompatible layouts! {} | {}",
            vk::to_string(this->layout),
            vk::to_string(from)
        );

        const vk::ImageMemoryBarrier barrier
        {   
            .sType               {vk::StructureType::eImageMemoryBarrier},
            .pNext               {nullptr},
            .srcAccessMask       {sourceAccess},
            .dstAccessMask       {destinationAccess},
            .oldLayout           {from},
            .newLayout           {to},
            .srcQueueFamilyIndex {VK_QUEUE_FAMILY_IGNORED},
            .dstQueueFamilyIndex {VK_QUEUE_FAMILY_IGNORED},
            .image               {this->image},
            .subresourceRange    {
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

        commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, nullptr, nullptr, barrier);

        this->layout = to;
    }


    void Image2D::copyFromBuffer(vk::CommandBuffer commandBuffer, const Buffer& buffer) const
    {
        std::size_t size_of_format;
        switch (this->format)
        {
            case vk::Format::eR32G32B32A32Sfloat:
                size_of_format = 16;
                break;
            case vk::Format::eR8G8B8A8Srgb:
                size_of_format = 4;
                break;
            case vk::Format::eD32Sfloat:
                size_of_format = 4;
                break;
            default:
                seb::panic("Unimplemented data type!");
        }
        
        const std::size_t totalBytes = this->extent.width * this->extent.height *  size_of_format;

        seb::assertFatal(
            totalBytes == buffer.sizeBytes(),
            "Incorrect sizes of buffers! {} : {}",
            totalBytes,
            buffer.sizeBytes()
        );

        const std::array<vk::BufferImageCopy, 1> region
        {
            vk::BufferImageCopy
            {
                .bufferOffset      {0},
                .bufferRowLength   {0},
                .bufferImageHeight {0},
                .imageSubresource  {this->aspect},
                .imageOffset       {},
                .imageExtent       {},
            }
        };

        commandBuffer.copyBufferToImage(*buffer, this->image, this->layout, region);

    }
    
} // namespace render