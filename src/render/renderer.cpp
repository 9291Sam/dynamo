#include "vulkan_includes.hpp"

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

        this->initializeRenderer();

        seb::logTrace("Renderer initalized successfully");
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

        this->descriptor_pool = std::make_unique<DescriptorPool>(
            this->device->asLogicalDevice(), this->MaxFramesInFlight
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
        this->descriptor_sets = this->descriptor_pool->allocate(this->pipeline->getDescriptorSetLayout());
        seb::assertFatal(
            this->descriptor_sets.size() == this->MaxFramesInFlight,
            "Incorrect number of Descriptor sets returned!"
        );

        // update
        for (std::size_t i = 0; i < this->MaxFramesInFlight; i++)
        {
            const vk::DescriptorBufferInfo uniformBufferBindingInfo
            {
                .buffer {**this->uniform_buffers.at(i)},
                .offset {0},
                .range  {sizeof(UniformBuffer)},
            };

            std::array<vk::WriteDescriptorSet, 1> writeInfo
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
                }
            };


            this->device->asLogicalDevice().updateDescriptorSets(writeInfo, nullptr);
        }


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