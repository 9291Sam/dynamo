#pragma once

#ifndef SRC_RENDER_FRAME_HPP
#define SRC_RENDER_FRAME_HPP

#include "vulkan_includes.hpp"

#include "pipeline.hpp"
#include "render_structs.hpp"
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
        vk::Result render(const Device&, const Swapchain&, const RenderPass&, const Pipeline&, 
            const std::vector<vk::UniqueFramebuffer>&, const std::vector<Object>&, const Camera&);

    private:
        vk::UniqueCommandBuffer command_buffer;
        vk::UniqueSemaphore     image_available;
        vk::UniqueSemaphore     render_finished;
        vk::UniqueFence         frame_in_flight;
    }; // class Frame
} // namespace render

#endif // SRC_RENDER_FRAME_HPP