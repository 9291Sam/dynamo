#ifndef SRC_RENDER_VULKAN_COMMAND__POOL_HPP
#define SRC_RENDER_VULKAN_COMMAND__POOL_HPP

#include "device.hpp"
#include "includes.hpp"

namespace render
{

    class CommandPool
    {
    public:

        CommandPool(const Device&);
        ~CommandPool()                             = default;

        CommandPool()                              = delete;
        CommandPool(const CommandPool&)            = delete;
        CommandPool(CommandPool&&)                 = default;
        CommandPool& operator=(const CommandPool&) = delete;
        CommandPool& operator=(CommandPool&&)      = default; 

        [[nodiscard]] vk::CommandPool operator*() const;

    private:
        vk::UniqueCommandPool command_pool;
    }; // class CommandPool

} // namespace render

#endif // SRC_RENDER_COMMAND__POOL_HPP