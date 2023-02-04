#include "vulkan_includes.hpp"

#include <sebib/seblog.hpp>

#include "renderer.hpp"

namespace render
{
    Renderer::Renderer(const Window& window)
        : instance     {nullptr}
        , draw_surface {nullptr}
        , device       {nullptr}
        , allocator    {nullptr}
        , command_pool {nullptr}
        , swapchain    {nullptr}
        , depth_buffer {nullptr}
        , render_pass  {nullptr}
        , pipeline     {nullptr}
        , framebuffers {0}
        , render_index {static_cast<std::size_t>(-1)}
        , frames       {nullptr, nullptr}
    {
        const vk::DynamicLoader dl;
        const PFN_vkGetInstanceProcAddr dynVkGetInstanceProcAddr = 
            dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(dynVkGetInstanceProcAddr);

        this->instance = std::make_unique<Instance>(dynVkGetInstanceProcAddr);

        VULKAN_HPP_DEFAULT_DISPATCHER.init(**this->instance);

        this->draw_surface = window.createSurface(**this->instance);

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

        this->swapchain = std::make_unique<Swapchain>(
            *this->device,
            *this->draw_surface,
            vk::Extent2D { // TODO: once we get this from the window, ensure its within range by modifying the wondows code to make sure its right
                .width {1200},
                .height {1200},
            }
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
            this->frames.at(i) = std::make_unique<Frame>(
                this->device->asLogicalDevice(), 
                std::move(commandBufferVector.at(i))
            );
        }

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

    bool Renderer::drawFrame(const Camera& camera, const std::vector<Object>& objects)
    {
        auto result = this->frames.at(this->render_index)->render(
            *this->device, *this->swapchain, *this->render_pass, *this->pipeline,
            this->framebuffers, objects, camera
        );

        this->render_index = (this->render_index + 1) % this->MaxFramesInFlight;

        switch (result)
        {
            case vk::Result::eSuccess:
                return true;
            case vk::Result::eErrorOutOfDateKHR:
                seb::panic("Resize unimplemented");
                return false;
            default:
                seb::panic("Draw frame failed result: {}", vk::to_string(result));
        }
    }

    void Renderer::resize(const Window& window)
    {
        seb::panic("unimplemented");
    }

} // namespace render