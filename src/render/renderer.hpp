#pragma once

#ifndef SRC_RENDER_RENDERER_HPP
#define SRC_RENDER_RENDERER_HPP

#include <ranges>

#include <sebib/seblog.hpp>

#include "allocator.hpp"
#include "command_pool.hpp"
#include "descriptor_pool.hpp"
#include "device.hpp"
#include "recorder.hpp"
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
    /// TODO: make a class that combines render_pass and frame buffers
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

            static float idx = 0.0f;

            idx += 0.001;

            // update Uniform Buffers TODO: refactor

            UniformBuffer uniformBuffer {
                .numberOfLights {1},
                ._padding {},
                .lights {
                    glm::vec4 {25 * std::cos(idx), 22.0 * std::cos(idx), 25 * std::sin(idx), 10.0f},
                    // glm::vec4 {30.0f, 8 + 3 * std::sin(idx), 10.0f, 100.0f},
                    // glm::vec4 {10.0f, -10.0f, 15.2f, 100.0f},
                    // glm::vec4 {10.0f, -8.0f, -10.0f, 100.0f},
                },
            };

            std::memcpy(
                this->uniform_buffers.at(this->render_index)->get_persistent_ptr(),
                &uniformBuffer,
                sizeof(UniformBuffer)
            );

            auto result = this->frames.at(this->render_index)->render(
                *this->device, *this->swapchain, *this->render_pass, *this->pipeline,
                this->framebuffers,
                *this->descriptor_sets.at(this->render_index),
                objectView, camera
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
        std::unique_ptr<Swapchain>      swapchain;
        std::unique_ptr<Image2D>        depth_buffer;
        std::unique_ptr<RenderPass>     render_pass;
        std::unique_ptr<Pipeline>       pipeline;
        std::unique_ptr<DescriptorPool> descriptor_pool; // one pool per thread


        // Frames in Flight
        std::vector<vk::UniqueFramebuffer>   framebuffers;


        // TODO: re-do the Recorder abstraction to properly handle multi threaded recording
        
        // Frame in flight
            // Each one can have a Recorder Thread

        // renderer
        std::size_t                                           render_index;
        constexpr static std::size_t                          MaxFramesInFlight = 2;
        std::array<std::unique_ptr<Buffer>, MaxFramesInFlight>   uniform_buffers;
        std::vector<vk::UniqueDescriptorSet> descriptor_sets;
        std::array<std::unique_ptr<Recorder>, MaxFramesInFlight> frames; // wait why the fuck do you need multiple frame buffers
        
    }; // class Renderer
} // namespace render

#endif // SRC_RENDER_RENDERER_HPP