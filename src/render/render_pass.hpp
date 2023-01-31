#pragma once

#ifndef SRC_RENDER_RENDER__PASS_HPP
#define SRC_RENDER_RENDER__PASS_HPP

#include "vulkan_includes.hpp"

#include "swapchain.hpp"
#include "image.hpp"

namespace render
{

    class RenderPass
    {
    public:

        RenderPass(vk::Device, const Swapchain&, const Image2D& depthBuffer);
        ~RenderPass()                            = default;

        RenderPass()                             = delete;
        RenderPass(const RenderPass&)            = delete;
        RenderPass(RenderPass&&)                 = delete;
        RenderPass& operator=(const RenderPass&) = delete;
        RenderPass& operator=(RenderPass&&)      = delete; 

        [[nodiscard, gnu::pure]] vk::RenderPass operator*() const;

    private:
        vk::UniqueRenderPass render_pass;
    }; // class RenderPass

} // namespace render

#endif // SRC_RENDER_RENDER__PASS_HPP