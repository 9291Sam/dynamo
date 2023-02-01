#pragma once

#ifndef SRC_RENDER_RENDERER_HPP
#define SRC_RENDER_RENDERER_HPP

#include "allocator.hpp"
#include "device.hpp"
#include "instance.hpp"
#include "pipeline.hpp"
#include "render_pass.hpp"
#include "image.hpp"
#include "swapchain.hpp"
#include "window.hpp"
#include "vulkan_includes.hpp"

namespace render
{
    /// @brief Abstraction over a vulkan instance with debug layers
    class Renderer
    {
    public:

        Renderer();
        ~Renderer()                          = default;

        Renderer(const Renderer&)            = delete;
        Renderer(Renderer&&)                 = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&)      = delete;

        //void drawFrame(CameraPosition, float cameraPitch, float cameraYaw, std::vector<Model> toDraw)

    private:
        Window window;
        std::unique_ptr<Instance>          instance;
        vk::UniqueSurfaceKHR               draw_surface;
        std::unique_ptr<Device>            device;
        std::unique_ptr<Allocator>         allocator;
        std::unique_ptr<Swapchain>         swapchain;
        std::unique_ptr<Image2D>           depth_buffer;
        std::unique_ptr<RenderPass>        render_pass;
        std::unique_ptr<Pipeline>          pipeline;
        std::vector<vk::UniqueFramebuffer> framebuffers;
    }; // class Renderer
} // namespace render

#endif // SRC_RENDER_RENDERER_HPP