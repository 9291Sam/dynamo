#pragma once

#ifndef SRC_RENDER_FRAME_HPP
#define SRC_RENDER_FRAME_HPP

#include "vulkan_includes.hpp"

#include "object.hpp"
#include "render_pass.hpp"
#include "swapchain.hpp"

namespace render
{
    class Frame
    {
    public:
        Frame(vk::Device, vk::UniqueCommandBuffer);
        ~Frame()                       = default;

        Frame()                        = delete;
        Frame(const Frame&)            = delete;
        Frame(Frame&&)                 = delete;
        Frame& operator=(const Frame&) = delete;
        Frame& operator=(Frame&&)      = delete;

        // TODO: add objects and cameras!
        vk::Result render(vk::Device, const Swapchain&, const RenderPass&, vk::Pipeline, 
            const std::vector<vk::UniqueFramebuffer>&, const std::vector<Object>&);

    private:
        vk::UniqueCommandBuffer command_buffer;
        vk::UniqueSemaphore     image_available;
        vk::UniqueSemaphore     render_finished;
        vk::UniqueFence         frame_in_flight;
    }; // class Frame
} // namespace render

#endif // SRC_RENDER_FRAME_HPP