#pragma once

#ifndef SRC_RENDER_RENDERER_HPP
#define SRC_RENDER_RENDERER_HPP

#include "allocator.hpp"
#include "command_pool.hpp"
#include "device.hpp"
#include "frame.hpp"
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
    /// TOOD: persistently mapped world data stuff
    /// TODO: Multiple depth buffers, currently they're racing
    /// move the frame buffer up
    /// Create a Frame class that holds all the command buffers and all the index stuff
    /// the goal is that you only get one index and index into one vector
    /// make sure header guards match
    /// Final member list should look like
    /// 
    /// Window window
    ///
    /// // Vulkan Initialization 
    /// Instance
    /// vk::UniqueSurfaceKHR
    /// Device
    /// Allocator
    /// CommandPool // Eventually one per thread submitting commands
    ///
    /// // Vulkan Rendering
    /// std::unique_ptr<Swapchain>
    /// std::unique_ptr<RenderPass>
    /// std::unique_ptr<Pipeline>
    /// std::size_t renderingIndex
    /// std::array<Frames, 2> frames
    ///
    class Renderer
    {
    public:

        Renderer(const Window&);
        ~Renderer();

        Renderer(const Renderer&)            = delete;
        Renderer(Renderer&&)                 = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&)      = delete;

        Object createObject(std::vector<Vertex>, std::optional<std::vector<Index>>) const;

        /// @brief draws given objects from the perspective of the given camera
        /// @return true onn success
        bool drawFrame(const Camera&, const std::vector<Object>&);
        void resize(const Window&);

    private:
        // TODO: split this up as follows
        // Window
        // Vulkan Instance
        // Vulkan Pipeline
        // Render thread

        // Vulkan Initialization 
        std::unique_ptr<Instance>    instance;
        vk::UniqueSurfaceKHR         draw_surface;
        std::unique_ptr<Device>      device;
        std::unique_ptr<Allocator>   allocator;
        std::unique_ptr<CommandPool> command_pool; // one pool per thread

        // Vulkan Rendering 
        std::unique_ptr<Swapchain>            swapchain;
        std::unique_ptr<Image2D>              depth_buffer;
        std::unique_ptr<RenderPass>           render_pass;
        std::unique_ptr<Pipeline>             pipeline;
        std::vector<vk::UniqueFramebuffer>    framebuffers;

        // Single threaded renderer
        std::size_t                                           render_index;
        constexpr static std::size_t                          MaxFramesInFlight = 2;
        std::array<std::unique_ptr<Frame>, MaxFramesInFlight> frames;
        
    }; // class Renderer
} // namespace render

#endif // SRC_RENDER_RENDERER_HPP