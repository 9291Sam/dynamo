#ifndef SRC_RENDER_RENDERER_HPP
#define SRC_RENDER_RENDERER_HPP

#include <set>

#include <sebib/seblog.hpp>
#include <sebib/sebmis.hpp>

#include "vulkan/allocator.hpp"
#include "vulkan/command_pool.hpp"
#include "vulkan/descriptor_pool.hpp"
#include "vulkan/device.hpp"
#include "recorder.hpp"
#include "vulkan/instance.hpp"
#include "vulkan/pipeline.hpp"
#include "vulkan/render_pass.hpp"
#include "vulkan/image.hpp"
#include "vulkan/swapchain.hpp"
#include "vulkan/includes.hpp"

#include "window.hpp"

namespace render
{
    /// @brief Abstraction over a vulkan instance with debug layers
    /// TOOD: persistently mapped world data stuff
    /// TODO: make a class that combines render_pass and frame buffers
    ///
    class Renderer
    {
    public:
        enum class Pipelines : std::size_t
        {
            FaceTexture = 0,
            WorldVoxels = 1,
            MAX_PIPELINE_SIZE = 2,
        };

        struct PipelinedObject
        {
            render::Renderer::Pipelines pipeline;
            render::Object              object;
        };
    private:
        using PipelineArray = std::array<
            Pipeline, 
            static_cast<std::size_t>(Pipelines::MAX_PIPELINE_SIZE)
        >;
    public:

        Renderer(vk::Extent2D defaultSize, std::string name);
        ~Renderer();

        Renderer(const Renderer&)            = delete;
        Renderer(Renderer&&)                 = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&)      = delete;

        // This function list is a mess TODO: redesign
        [[nodiscard]] Object createObject(std::vector<Vertex>, std::optional<std::vector<Index>>) const;
        [[nodiscard]] auto getKeyCallback() const -> std::function<bool(vkfw::Key)>;
        [[nodiscard]] std::pair<double, double> getMouseDelta();
        [[nodiscard]] float getDeltaTimeSeconds() const;
        [[nodiscard]] bool shouldClose() const;

        void attachCursor() const;
        void detachCursor() const;
        
        void drawFrame(const Camera& camera, const std::vector<PipelinedObject>& objectView);
        
        void resize();

    private:
        void initializeRenderer();


        Window window;
        std::queue<std::function<void(vk::CommandBuffer)>> extra_commands;

        // Vulkan Initialization 
        std::unique_ptr<Instance>    instance;
        vk::UniqueSurfaceKHR         draw_surface;
        std::unique_ptr<Device>      device;
        std::unique_ptr<Allocator>   allocator;
        std::unique_ptr<CommandPool> command_pool; // one pool per thread

        // scratch stuff
        std::unique_ptr<Buffer>  image_buffer;
        std::unique_ptr<Image2D> texture;
        vk::UniqueSampler        texture_sampler;

        // Vulkan Rendering 
        std::unique_ptr<Swapchain>      swapchain;
        std::unique_ptr<Image2D>        depth_buffer;
        std::unique_ptr<RenderPass>     render_pass;
        std::unique_ptr<PipelineArray>  pipelines; 
        std::unique_ptr<DescriptorPool> descriptor_pool; // one pool per thread
        
        // Frames in Flight
        std::vector<vk::UniqueFramebuffer>   framebuffers;

        // renderer
        std::size_t                                              render_index;
        constexpr static std::size_t                             MaxFramesInFlight = 2;
        std::array<std::unique_ptr<Buffer>, MaxFramesInFlight>   uniform_buffers;
        std::vector<vk::UniqueDescriptorSet>                     descriptor_sets;
        std::array<std::unique_ptr<Recorder>, MaxFramesInFlight> frames;
        
    }; // class Renderer
} // namespace render

#endif // SRC_RENDER_RENDERER_HPP