#pragma once

#ifndef SRC_RENDER_RENDERER_HPP
#define SRC_RENDER_RENDERER_HPP

#include <ranges>

#include <sebib/seblog.hpp>

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
    /// TODO: allow referencesto be stored to static members
    /// Create a Frame class that holds all the command buffers and all the index stuff
    /// the goal is that you only get one index and index into one vector
    /// make sure header guards match
    /// Final member list should look like
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

        Renderer(vk::Extent2D defaultSize, std::string name);
        ~Renderer();

        Renderer(const Renderer&)            = delete;
        Renderer(Renderer&&)                 = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&)      = delete;

        // This function list is a mess TODO: redesign
        [[nodiscard, gnu::pure]] Object createObject(std::vector<Vertex>, std::optional<std::vector<Index>>) const;
        [[nodiscard, gnu::const]] auto getKeyCallback() const -> std::function<bool(vkfw::Key)>;
        [[nodiscard, gnu::pure]] float getDeltaTimeSeconds() const;
        [[nodiscard, gnu::pure]] bool shouldClose() const;

        void drawFrame(const Camera& camera, std::ranges::input_range auto& objectView)
        {
            this->window.pollEvents();
            auto result = this->frames.at(this->render_index)->render(
                *this->device, *this->swapchain, *this->render_pass, *this->pipeline,
                this->framebuffers, objectView, camera
            );

            this->render_index = (this->render_index + 1) % this->MaxFramesInFlight;

            switch (result)
            {
                case vk::Result::eSuccess:
                    return;
                case vk::Result::eErrorOutOfDateKHR:
                    this->resize();
                    return;
                default:
                    seb::panic("Draw frame failed result: {}", vk::to_string(result));
            }
        }
        
        void resize();

    private:
        void initializeRenderer();


        Window window;

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