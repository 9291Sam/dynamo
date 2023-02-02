#include "vulkan_includes.hpp"

#include <sebib/seblog.hpp>

#include "renderer.hpp"

namespace render
{
    Renderer::Renderer()
        : window       {{1200, 1200}, "Dynamo"}
        , instance     {nullptr}
        , draw_surface {nullptr}
        , device       {nullptr}
        , allocator    {nullptr}
        , swapchain    {nullptr}
        , depth_buffer {nullptr}
        , render_pass  {nullptr}
        , pipeline     {nullptr}
        , framebuffers {0}
        , command_pool {nullptr}
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

        this->command_pool = std::make_unique<CommandPool>(*this->device);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);

        seb::todo<>("End of execution!");
    }

} // namespace render