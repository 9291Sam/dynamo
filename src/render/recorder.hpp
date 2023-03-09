#ifndef SRC_RENDER_RECORDER_HPP
#define SRC_RENDER_RECORDER_HPP

#include <set>
#include <queue>

#include <sebib/sebmis.hpp>

#include "vulkan/pipeline.hpp"
#include "vulkan/swapchain.hpp"
#include "vulkan/includes.hpp"
#include "vulkan/render_pass.hpp"

#include "render_structs.hpp"

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
            const Device&, const Swapchain&, const RenderPass&,
            const std::vector<vk::UniqueFramebuffer>&, vk::DescriptorSet,
            const std::vector<std::pair<const Pipeline*, std::vector<const Object*>>>&,
            const Camera&, 
            std::queue<std::function<void(vk::CommandBuffer)>>&
        );

    private:
        vk::UniqueCommandBuffer command_buffer;
        vk::UniqueSemaphore     image_available;
        vk::UniqueSemaphore     render_finished;
        vk::UniqueFence         frame_in_flight;
    }; // class Recorder
} // namespace render

#endif // SRC_RENDER_RECORDER_HPP