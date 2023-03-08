#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <sebib/seblog.hpp>

#include "renderer.hpp"

namespace render
{
    Renderer::Renderer(vk::Extent2D size, std::string name)
        : window       {size, name}
        , instance     {nullptr}
        , draw_surface {nullptr}
        , device       {nullptr}
        , allocator    {nullptr}
        , command_pool {nullptr}
        , image_buffer {nullptr}
        , texture      {nullptr}
        , swapchain    {nullptr}
        , depth_buffer {nullptr}
        , render_pass  {nullptr}
        , pipeline     {nullptr}
        , framebuffers {0}
        , render_index {0}
        , frames       {nullptr, nullptr}
    {
        const vk::DynamicLoader dl;
        const PFN_vkGetInstanceProcAddr dynVkGetInstanceProcAddr = 
            dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(dynVkGetInstanceProcAddr);

        this->instance = std::make_unique<Instance>(dynVkGetInstanceProcAddr);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(**this->instance);

        this->draw_surface = this->window.createSurface(**this->instance);

        this->device = std::make_unique<Device>(**this->instance, *this->draw_surface);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(**this->instance, this->device->asLogicalDevice());

        this->command_pool = std::make_unique<CommandPool>(*this->device);

        this->allocator = std::make_unique<Allocator>(
            **this->instance,
            this->device->asPhysicalDevice(),
            this->device->asLogicalDevice(),
            dynVkGetInstanceProcAddr,
            dl.getProcAddress<PFN_vkGetDeviceProcAddr>("vkGetDeviceProcAddr")
        );

        // this->texture && this->texture_sampler initalization
        this->extra_commands.push([&](vk::CommandBuffer commandBuffer)
        {
            int width;
            int height;
            int textureChannels;

            std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> pixels {
                stbi_load("../textures/face.jpeg", &width, &height, &textureChannels, STBI_rgb_alpha),
                stbi_image_free
            };

            seb::assertFatal(pixels.get(), "Failed to load from file, is the filepath correct?");

            const vk::DeviceSize imageSize = width * height * 4; // 4 is for the rgba component being 4 bytes;

            this->image_buffer = std::make_unique<Buffer>(
                **this->allocator,
                imageSize,
                vk::BufferUsageFlagBits::eTransferSrc,
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            );

            this->image_buffer->write(std::span<std::byte>{reinterpret_cast<std::byte*>(pixels.get()), imageSize});
    
            this->texture = std::make_unique<Image2D>(
                *this->allocator,
                this->device->asLogicalDevice(),
                vk::Extent2D
                {
                    .width  {static_cast<std::uint32_t>(width)},
                    .height {static_cast<std::uint32_t>(height)}
                },
                vk::Format::eR8G8B8A8Srgb,
                vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
                vk::ImageAspectFlagBits::eColor,
                vk::ImageTiling::eOptimal,
                vk::MemoryPropertyFlagBits::eDeviceLocal
            );

            this->texture->transitionLayout(
                commandBuffer,
                vk::ImageLayout::eUndefined,
                vk::ImageLayout::eTransferDstOptimal,
                vk::PipelineStageFlagBits::eTopOfPipe,
                vk::PipelineStageFlagBits::eTransfer,
                vk::AccessFlagBits::eNone,
                vk::AccessFlagBits::eTransferWrite
            );
            this->texture->copyFromBuffer(commandBuffer, *this->image_buffer);
            this->texture->transitionLayout(
                commandBuffer,
                vk::ImageLayout::eTransferDstOptimal,
                vk::ImageLayout::eShaderReadOnlyOptimal,
                vk::PipelineStageFlagBits::eTransfer,
                vk::PipelineStageFlagBits::eFragmentShader,
                vk::AccessFlagBits::eTransferWrite,
                vk::AccessFlagBits::eShaderRead
            );

            const vk::SamplerCreateInfo samplerCreateInfo
            {
                .sType                   {vk::StructureType::eSamplerCreateInfo},
                .pNext                   {nullptr},
                .flags                   {},
                .magFilter               {VULKAN_HPP_NAMESPACE::Filter::eNearest},
                .minFilter               {VULKAN_HPP_NAMESPACE::Filter::eNearest},
                .mipmapMode              {VULKAN_HPP_NAMESPACE::SamplerMipmapMode::eNearest},
                .addressModeU            {VULKAN_HPP_NAMESPACE::SamplerAddressMode::eRepeat},
                .addressModeV            {VULKAN_HPP_NAMESPACE::SamplerAddressMode::eRepeat},
                .addressModeW            {VULKAN_HPP_NAMESPACE::SamplerAddressMode::eRepeat},
                .mipLodBias              {},
                .anisotropyEnable        {true},
                .maxAnisotropy           {this->device->asPhysicalDevice().getProperties().limits.maxSamplerAnisotropy},
                .compareEnable           {false},
                .compareOp               {VULKAN_HPP_NAMESPACE::CompareOp::eAlways},
                .minLod                  {},
                .maxLod                  {},
                .borderColor             {VULKAN_HPP_NAMESPACE::BorderColor::eIntOpaqueBlack},
                .unnormalizedCoordinates {false},
            };

            this->texture_sampler = this->device->asLogicalDevice().createSamplerUnique(samplerCreateInfo);
        });
        this->initializeRenderer();

        seb::logLog("Renderer initalized successfully");
    }

    Renderer::~Renderer()
    {
        this->device->asLogicalDevice().waitIdle();
    }

    Object Renderer::createObject(std::vector<Vertex> v, std::optional<std::vector<Index>> i) const
    {
        return Object {
            **this->allocator,
            std::forward<std::vector<Vertex>>(v),
            std::forward<std::optional<std::vector<Index>>>(i)
        };
    }

    std::pair<double, double> Renderer::getMouseDelta()
    {
        return this->window.getMouseDelta();
    }

    bool Renderer::shouldClose() const
    {
        return this->window.shouldClose();
    }

    std::function<bool(vkfw::Key)> Renderer::getKeyCallback() const
    {
        return [this](vkfw::Key key) -> bool
        {
            return this->window.isKeyPressed(key);
        };
    }

    float Renderer::getDeltaTimeSeconds() const
    {
        return this->window.getDeltaTimeSeconds();
    }

    void Renderer::drawFrame(const Camera& camera, const std::vector<Object>& objectView)
    {
            this->window.pollEvents();

            static float idx = 0.0f;

            idx += 5.5f * this->getDeltaTimeSeconds();

            // update Uniform Buffers TODO: refactor

            UniformBuffer uniformBuffer {
                .light_position {50 * std::cos(idx / 7.0f), 7.0f * std::sin(idx / 9.0f) + 15.0f, 50.0f * std::sin(idx / 11.0f)},
                .light_color {1.0f, 1.0f, 1.0f, 10.0f}
            };

            std::memcpy(
                this->uniform_buffers.at(this->render_index)->getMappedPtr(),
                &uniformBuffer,
                sizeof(UniformBuffer)
            );

            std::vector<std::pair<const Pipeline&, const std::vector<Object>&>> objects;

            objects.push_back({*this->pipeline, objectView});

            auto result = this->frames.at(this->render_index)->render(
                *this->device, *this->swapchain, *this->render_pass,
                this->framebuffers,
                *this->descriptor_sets.at(this->render_index),
                objects,
                camera, this->extra_commands
            );

            this->render_index = (this->render_index + 1) % this->MaxFramesInFlight;

            switch (result)
            {
                case vk::Result::eSuccess:
                    return;
                case vk::Result::eErrorOutOfDateKHR:
                    this->resize();
                    return;
                default:
                    seb::panic("Draw frame failed result: {}", vk::to_string(result));
            }
        }

    void Renderer::resize()
    {
        seb::logTrace("Renderer resizimg!");

        // Sanity checks
        this->window.blockThisThreadIfMinimized();
        this->device->asLogicalDevice().waitIdle();

        for (std::unique_ptr<Recorder>& f : this->frames)
        {
            f.reset();
        }
        this->descriptor_sets.clear();
        for (std::unique_ptr<Buffer>& b : this->uniform_buffers)
        {
            b.reset();
        }
        this->framebuffers.clear();
        this->descriptor_pool.reset();
        this->pipeline.reset();
        this->render_pass.reset(); // TODO: does this need to be reset?
        this->depth_buffer.reset();
        this->swapchain.reset();
 
        this->initializeRenderer();
    }

    void Renderer::initializeRenderer()
    {
        this->swapchain = std::make_unique<Swapchain>(
            *this->device,
            *this->draw_surface,
            this->window.size()
        );

        this->depth_buffer = std::make_unique<Image2D>(
            *this->allocator,
            this->device->asLogicalDevice(),
            this->swapchain->getExtent(),
            vk::Format::eD32Sfloat,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::ImageAspectFlagBits::eDepth,
            vk::ImageTiling::eOptimal,
            vk::MemoryPropertyFlagBits::eDeviceLocal
        );

        this->render_pass = std::make_unique<RenderPass>(
            this->device->asLogicalDevice(),
            *this->swapchain,
            *this->depth_buffer
        );

        this->pipeline = std::make_unique<Pipeline>(
            this->device->asLogicalDevice(),
            **this->render_pass,
            this->swapchain->getExtent(),
            Pipeline::createShaderFromFile(
                this->device->asLogicalDevice(),
                "src/render/shaders/shader.vert.bin"
            ),
            Pipeline::createShaderFromFile(
                this->device->asLogicalDevice(),
                "src/render/shaders/shader.frag.bin"
            )
        );

        seb::logWarn("Unhardcode");
        this->descriptor_pool = std::make_unique<DescriptorPool>(
            this->device->asLogicalDevice(),
            this->pipeline->getDescriptorSetLayout(),
            std::vector {
                vk::DescriptorPoolSize
                {
                    .type            {vk::DescriptorType::eUniformBuffer},
                    .descriptorCount {static_cast<std::uint32_t>(this->MaxFramesInFlight)}
                },
                vk::DescriptorPoolSize
                {
                    .type            {vk::DescriptorType::eCombinedImageSampler},
                    .descriptorCount {static_cast<std::uint32_t>(this->MaxFramesInFlight)}
                }
            }
        );

        // framebuffer creation
        for (const vk::UniqueImageView& view : this->swapchain->getImageViews())
        {
            std::array<vk::ImageView, 2> attachments
            {
                *view,
                **this->depth_buffer
            };

            vk::FramebufferCreateInfo frameBufferCreateInfo {
                .sType           {vk::StructureType::eFramebufferCreateInfo},
                .pNext           {nullptr},
                .flags           {},
                .renderPass      {**this->render_pass},
                .attachmentCount {attachments.size()},
                .pAttachments    {attachments.data()},
                .width           {this->swapchain->getExtent().width},
                .height          {this->swapchain->getExtent().height},
                .layers          {1},
            };

            this->framebuffers.push_back(
                this->device->asLogicalDevice()
                    .createFramebufferUnique(frameBufferCreateInfo)
            );
        }

        // uniform buffer creation
        for (std::size_t i = 0; i < this->MaxFramesInFlight; ++i)
        {
            this->uniform_buffers.at(i) = std::make_unique<Buffer>(
                **this->allocator,
                sizeof(UniformBuffer),
                vk::BufferUsageFlagBits::eUniformBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            );
        }

        // bind uniform buffers to descriptor sets
        // allocate
        this->descriptor_sets = this->descriptor_pool->allocate();
        seb::assertFatal(
            this->descriptor_sets.size() == this->MaxFramesInFlight,
            "Incorrect number of Descriptor sets returned!"
        );

        // bind descriptorsets to their corresponding buffers
        // this is in an extra command since it needs to be done after theyre created
        this->extra_commands.push([&]([[maybe_unused]] vk::CommandBuffer)
        {
            for (std::size_t i = 0; i < this->MaxFramesInFlight; i++)
            {
                const vk::DescriptorBufferInfo uniformBufferBindingInfo
                {
                    .buffer {**this->uniform_buffers.at(i)},
                    .offset {0},
                    .range  {sizeof(UniformBuffer)},
                };

                const vk::DescriptorImageInfo textureImageBindingInfo
                {
                    .sampler     {*this->texture_sampler},
                    .imageView   {**this->texture},
                    // this isn't the image's current state, it's the final state it will be in
                    .imageLayout {vk::ImageLayout::eShaderReadOnlyOptimal},
                };

                std::array<vk::WriteDescriptorSet, 2> writeInfo
                {
                    vk::WriteDescriptorSet
                    {
                        .sType            {vk::StructureType::eWriteDescriptorSet},
                        .pNext            {nullptr},
                        .dstSet           {*this->descriptor_sets.at(i)},
                        .dstBinding       {0},
                        .dstArrayElement  {0},
                        .descriptorCount  {1},
                        .descriptorType   {vk::DescriptorType::eUniformBuffer},
                        .pImageInfo       {nullptr},
                        .pBufferInfo      {&uniformBufferBindingInfo},
                        .pTexelBufferView {nullptr},
                    },
                    vk::WriteDescriptorSet
                    {
                        .sType            {vk::StructureType::eWriteDescriptorSet},
                        .pNext            {nullptr},
                        .dstSet           {*this->descriptor_sets.at(i)},
                        .dstBinding       {1},
                        .dstArrayElement  {0},
                        .descriptorCount  {1},
                        .descriptorType   {vk::DescriptorType::eCombinedImageSampler},
                        .pImageInfo       {&textureImageBindingInfo},
                        .pBufferInfo      {nullptr},
                        .pTexelBufferView {nullptr},
                    },
                };

                this->device->asLogicalDevice().updateDescriptorSets(writeInfo, nullptr);
            }
        });
        


        // recreate frames
        const vk::CommandBufferAllocateInfo commandBuffersAllocateInfo
        {
            .sType              {vk::StructureType::eCommandBufferAllocateInfo},
            .pNext              {},
            .commandPool        {**this->command_pool},
            .level              {vk::CommandBufferLevel::ePrimary},
            .commandBufferCount {static_cast<std::uint32_t>(this->MaxFramesInFlight)},
        };

        auto commandBufferVector = 
            this->device->asLogicalDevice()
                .allocateCommandBuffersUnique(commandBuffersAllocateInfo);

        for (std::size_t i = 0; i < this->MaxFramesInFlight; ++i)
        {
            this->frames.at(i) = std::make_unique<Recorder>(
                this->device->asLogicalDevice(), 
                std::move(commandBufferVector.at(i))
            );
        }
    }

} // namespace render