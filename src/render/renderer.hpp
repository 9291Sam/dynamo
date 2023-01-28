#pragma once

#ifndef SRC_RENDER_RENDERER_HPP
#define SRC_RENDER_RENDERER_HPP

#include "device.hpp"
#include "instance.hpp"
#include "window.hpp"

namespace render
{
    /// @brief Abstraction over a vulkan instance with debug layers
    class Renderer
    {
    public:

        Renderer();
        ~Renderer();

        Renderer(const Renderer&)            = delete;
        Renderer(Renderer&&)                 = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&)      = delete;

    private:
        Window window;

        std::unique_ptr<Instance> instance;
        std::unique_ptr<Device> device;
        vk::UniqueSurfaceKHR draw_surface;
        
    }; // class Renderer
} // namespace render

#endif // SRC_RENDER_RENDERER_HPP