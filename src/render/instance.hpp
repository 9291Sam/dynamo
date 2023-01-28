#pragma once

#ifndef SRC_RENDER_INSTANCE_HPP
#define SRC_RENDER_INSTANCE_HPP

#include "vulkan_includes.hpp"

namespace render
{
    /// @brief Abstraction over a vulkan instance with debug layers
    class Instance
    {
    public:

        Instance();
        ~Instance();

        Instance(const Instance&)            = delete;
        Instance(Instance&&)                 = delete;
        Instance& operator=(const Instance&) = delete;
        Instance& operator=(Instance&&)      = delete;

        [[nodiscard]] vk::Instance operator*() const;

    private:
        vk::UniqueInstance instance;
        VkDebugUtilsMessengerEXT debug_messenger;
    } // class Instance
} // namespace render

#endif // SRC_RENDER_INSTANCE_HPP