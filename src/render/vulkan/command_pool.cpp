#include "command_pool.hpp"

namespace render
{
    CommandPool::CommandPool(const Device& device)
    {
        vk::CommandPoolCreateInfo commandPoolCreateInfo
        {
            .sType            {vk::StructureType::eCommandPoolCreateInfo},
            .pNext            {nullptr},
            .flags            {vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
            .queueFamilyIndex {device.getRenderComputeTransferIndex()}
        };

        this->command_pool = device.asLogicalDevice()
            .createCommandPoolUnique(commandPoolCreateInfo);
    }

    vk::CommandPool CommandPool::operator*() const
    {
        return *this->command_pool;
    }
} // namespace render