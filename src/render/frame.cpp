#include <sebib/seblog.hpp>

#include "gpu_structs.hpp"
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

    vk::Result Frame::render(const Device& device, const Swapchain& swapchain, const RenderPass& renderPass,
        const Pipeline& pipeline, const std::vector<vk::UniqueFramebuffer>& framebuffers, 
        const std::vector<Object>& objectsToDraw, const Camera& camera)
    {
        const auto timeout = std::numeric_limits<std::uint64_t>::max();

        auto result = device.asLogicalDevice().waitForFences(*this->frame_in_flight, true, timeout);
        seb::assertFatal(
            result == vk::Result::eSuccess || result == vk::Result::eTimeout,
            "Failed to wait for render fence {}", vk::to_string(result)
        );

        const auto [result1, maybeNextIdx] = device.asLogicalDevice()
            .acquireNextImageKHR(*swapchain, timeout, *this->image_available);
        seb::assertFatal(result1 == vk::Result::eSuccess, "Failed to acquire next Image {}", vk::to_string(result));

        device.asLogicalDevice().resetFences(*this->frame_in_flight);

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
        this->command_buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);

        for (const Object& o : objectsToDraw)
        {
            o.bind(this->command_buffer.get());

            std::array<PushConstants, 1> pushConstants {
                PushConstants
                {
                    .model_view_projection {
                        Camera::getPerspectiveMatrix(
                            glm::radians(70.f),
                            static_cast<float>(swapchain.getExtent().width) / 
                            static_cast<float>(swapchain.getExtent().height),
                            0.1f,
                            200.0f
                        ) * 
                        camera.asViewMatrix() * 
                        o.transform.asModelMatrix()
                    }
                },
            };
            

            this->command_buffer->pushConstants<render::PushConstants>(
                pipeline.getLayout(),
                vk::ShaderStageFlagBits::eVertex,
                0,
                pushConstants
            );

            o.draw(this->command_buffer.get());
        }

        this->command_buffer->endRenderPass();
        this->command_buffer->end();



        // Submission to graphics card
        const vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    
        std::array<vk::SubmitInfo, 1> submitInfos
        {
            vk::SubmitInfo
            {
                .sType                {vk::StructureType::eSubmitInfo},
                .pNext                {nullptr},
                .waitSemaphoreCount   {1},
                .pWaitSemaphores      {&*this->image_available},
                .pWaitDstStageMask    {&waitStages},
                .commandBufferCount   {1},
                .pCommandBuffers      {&*this->command_buffer}, 
                .signalSemaphoreCount {1},
                .pSignalSemaphores    {&*this->render_finished},
            }
        };

        device.getRenderQueue().submit(submitInfos, *this->frame_in_flight);

        vk::SwapchainKHR swapchainPtr = *swapchain;

        vk::PresentInfoKHR presentInfo
        {
            .sType              {vk::StructureType::ePresentInfoKHR},
            .pNext              {nullptr},
            .waitSemaphoreCount {1},
            .pWaitSemaphores    {&*this->render_finished},
            .swapchainCount     {1},
            .pSwapchains        {&swapchainPtr},
            .pImageIndices      {&maybeNextIdx},
            .pResults           {nullptr},
        };

        // also present queue TODO: fix
        auto result2 = device.getRenderQueue().presentKHR(presentInfo);

        seb::assertFatal(
            device.asLogicalDevice().waitForFences(
                *this->frame_in_flight,
                true,
                std::numeric_limits<std::uint64_t>::max()
            ) == vk::Result::eSuccess,
            "Failed to wait for frame to complete drawing"
        );

        switch (result2)
        {
            case vk::Result::eSuccess:
                return vk::Result::eSuccess;

            case vk::Result::eSuboptimalKHR:
            case vk::Result::eErrorOutOfDateKHR:
                return vk::Result::eErrorOutOfDateKHR;

            default:
                seb::panic(
                    "Failed to present image Result: {}",
                    vk::to_string(vk::Result {result})
                );
                break;
        }
    }

} // namespace render

