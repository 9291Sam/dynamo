#include <sebib/seblog.hpp>

#include "data_formats.hpp"
#include "frame.hpp"

namespace render
{
    Frame::Frame(vk::Device device, vk::UniqueCommandBuffer commandBuffer)
        : command_buffer {std::move(commandBuffer)}
    {
        const vk::SemaphoreCreateInfo semaphoreCreateInfo
        {
            .sType {vk::StructureType::eSemaphoreCreateInfo},
            .pNext {nullptr},
            .flags {},
        };

        const vk::FenceCreateInfo fenceCreateInfo
        {
            .sType {vk::StructureType::eFenceCreateInfo},
            .pNext {nullptr},
            .flags {vk::FenceCreateFlagBits::eSignaled},
        };

        this->image_available = device.createSemaphoreUnique(semaphoreCreateInfo);
        this->render_finished = device.createSemaphoreUnique(semaphoreCreateInfo);
        this->frame_in_flight = device.createFenceUnique(fenceCreateInfo);
    }

    vk::Result Frame::render(vk::Device device, const Swapchain& swapchain, const RenderPass& renderPass,
        vk::Pipeline pipeline, const std::vector<vk::UniqueFramebuffer>& framebuffers, 
        const std::vector<Object>& objectsToDraw)
    {
        const auto timeout = std::numeric_limits<std::uint64_t>::max();

        device.waitForFences(*this->frame_in_flight, true, timeout);

        const auto [result, maybeNextIdx] = device.acquireNextImageKHR(*swapchain, timeout, *this->image_available);
        seb::assertFatal(result == vk::Result::eSuccess, "Failed to acquire next Image {}", vk::to_string(result));

        device.resetFences(*this->frame_in_flight);

        this->command_buffer->reset();

        const vk::CommandBufferBeginInfo commandBufferBeginInfo
        {
            .sType            {vk::StructureType::eCommandBufferBeginInfo},
            .pNext            {nullptr},
            .flags            {},
            .pInheritanceInfo {nullptr},
        };

        this->command_buffer->begin(commandBufferBeginInfo);

        std::array<vk::ClearValue, 2> clearValues
        {
            vk::ClearValue
            {
                .color {vk::ClearColorValue {std::array<float, 4>{0.0f, 0.7f, 0.7f, 1.0f}}}
            },
            vk::ClearValue
            {
                .depthStencil {
                    vk::ClearDepthStencilValue {
                        .depth {1.0f},
                        .stencil {0}
                    }
                }
            }
        };

        vk::RenderPassBeginInfo renderPassBeginInfo
        {
            .sType           {vk::StructureType::eRenderPassBeginInfo},
            .pNext           {nullptr},
            .renderPass      {*renderPass},
            .framebuffer     {*framebuffers.at(maybeNextIdx)},
            .renderArea      
            {
                vk::Rect2D 
                {
                    .offset {0, 0},
                    .extent {swapchain.getExtent()},
                }
            },
            .clearValueCount {clearValues.size()},
            .pClearValues    {clearValues.data()},
        };

        this->command_buffer->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
        this->command_buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

        for (const Object& o : objectsToDraw)
        {
            o.bind(*this->command_buffer);

            PushConst
        }


    }
} // namespace render

