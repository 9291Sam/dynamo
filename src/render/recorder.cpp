#include <sebib/seblog.hpp>

#include "gpu_structs.hpp"
#include "recorder.hpp"

namespace render
{
    Recorder::Recorder(vk::Device device, vk::UniqueCommandBuffer commandBuffer)
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

    vk::Result Recorder::render(
        const Device& device, const Swapchain& swapchain,
        const RenderPass& renderPass, const Pipeline& pipeline, 
        const std::vector<vk::UniqueFramebuffer>& framebuffers, 
        vk::DescriptorSet descriptorSet,
        const std::vector<Object>& objectsToDraw, const Camera& camera,
        std::queue<seb::Fn<void(vk::CommandBuffer)>>& extraCommandsQueue)
    {
        // seb::logWarn("BIND UNIFORMB UFF");
        const auto timeout = std::numeric_limits<std::uint64_t>::max();

        auto result = device.asLogicalDevice().waitForFences(*this->frame_in_flight, true, timeout);
        seb::assertFatal(
            result == vk::Result::eSuccess || result == vk::Result::eTimeout,
            "Failed to wait for render fence {}", vk::to_string(result)
        );

        const auto [result1, maybeNextIdx] = device.asLogicalDevice()
            .acquireNextImageKHR(*swapchain, timeout, *this->image_available);

        if (result1 == vk::Result::eErrorOutOfDateKHR || result1 == vk::Result::eSuboptimalKHR)
        {
            return vk::Result::eErrorOutOfDateKHR;
        }
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

        while (!extraCommandsQueue.empty())
        {
            extraCommandsQueue.front()(*this->command_buffer);

            extraCommandsQueue.pop();
        }

        std::array<vk::ClearValue, 2> clearValues
        {
            vk::ClearValue
            {
                .color {vk::ClearColorValue {std::array<float, 4>{0.02f, 0.02f, 0.02f, 1.0f}}}
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
                    .view_projection {
                        Camera::getPerspectiveMatrix(
                            glm::radians(70.f),
                            static_cast<float>(swapchain.getExtent().width) / 
                            static_cast<float>(swapchain.getExtent().height),
                            0.1f,
                            200000.0f
                        ) * 
                        camera.asViewMatrix()
                    },
                    .model {
                        o.transform.asModelMatrix()
                    }
                },
            };
            

            this->command_buffer->pushConstants<render::PushConstants>(
                pipeline.getLayout(),
                vk::ShaderStageFlagBits::eAllGraphics,
                0,
                pushConstants
            );

            this->command_buffer->bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                pipeline.getLayout(), 
                0,
                std::array<vk::DescriptorSet, 1> {descriptorSet},
                nullptr
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

        try
        {
            (void)device.getRenderQueue().presentKHR(presentInfo);
        }
        catch (vk::OutOfDateKHRError& e)
        {
            return vk::Result::eErrorOutOfDateKHR;
        }

        seb::assertFatal(
            device.asLogicalDevice().waitForFences(
                *this->frame_in_flight,
                true,
                std::numeric_limits<std::uint64_t>::max()
            ) == vk::Result::eSuccess,
            "Failed to wait for frame to complete drawing"
        );

        return vk::Result::eSuccess;
    }

} // namespace render

