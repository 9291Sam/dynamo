#ifndef SRC_RENDER_COMMAND__POOL_HPP
#define SRC_RENDER_COMMAND__POOL_HPP

#include "device.hpp"
#include "vulkan_includes.hpp"

namespace render
{

    class CommandPool
    {
    public:

        CommandPool(const Device&);
        ~CommandPool()                             = default;

        CommandPool()                              = delete;
        CommandPool(const CommandPool&)            = delete;
        CommandPool(CommandPool&&)                 = delete;
        CommandPool& operator=(const CommandPool&) = delete;
        CommandPool& operator=(CommandPool&&)      = delete; 

        [[nodiscard, gnu::pure]] vk::CommandPool operator*() const;

    private:
        vk::UniqueCommandPool command_pool;

    }; // class CommandPool

} // namespace render

#endif // SRC_RENDER_COMMAND__POOL_HPP