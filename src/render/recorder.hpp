#pragma once

#ifndef SRC_RENDER_RECORDER_HPP
#define SRC_RENDER_RECORDER_HPP

#include <queue>

#include <sebib/sebmis.hpp>

#include "pipeline.hpp"
#include "render_structs.hpp"
#include "render_pass.hpp"
#include "swapchain.hpp"
#include "vulkan_includes.hpp"

namespace render
{
    class Recorder
    {
    public:
        Recorder(vk::Device, vk::UniqueCommandBuffer);
        ~Recorder()                       = default;

        Recorder()                        = delete;
        Recorder(const Recorder&)            = delete;
        Recorder(Recorder&&)                 = delete;
        Recorder& operator=(const Recorder&) = delete;
        Recorder& operator=(Recorder&&)      = delete;

        vk::Result render(
            const Device&, const Swapchain&, const RenderPass&, const Pipeline&, 
            const std::vector<vk::UniqueFramebuffer>&, vk::DescriptorSet,
            const std::vector<Object>&, const Camera&, 
            std::queue<seb::Fn<void(vk::CommandBuffer)>>&
        );

    private:
        vk::UniqueCommandBuffer command_buffer;
        vk::UniqueSemaphore     image_available;
        vk::UniqueSemaphore     render_finished;
        vk::UniqueFence         frame_in_flight;
    }; // class Recorder
} // namespace render

#endif // SRC_RENDER_RECORDER_HPP