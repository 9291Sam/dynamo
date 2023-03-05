#include "descriptor_pool.hpp"

namespace render
{

    DescriptorPool::DescriptorPool(
        vk::Device device_, 
        vk::DescriptorSetLayout layout_,
        const std::vector<vk::DescriptorPoolSize>& pools
    )
        : device {device_}
        , layout {layout_}
        , number_of_sets {pools.size()}
    {    
        const vk::DescriptorPoolCreateInfo poolCreateInfo
        {
            .sType         {vk::StructureType::eDescriptorPoolCreateInfo},
            .pNext         {nullptr},
            .flags         {vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet},
            .maxSets       {static_cast<std::uint32_t>(this->number_of_sets)},
            .poolSizeCount {static_cast<std::uint32_t>(pools.size())},
            .pPoolSizes    {pools.data()}
        };

        this->pool = device.createDescriptorPoolUnique(poolCreateInfo);
    }

    auto DescriptorPool::allocate() const
        -> std::vector<vk::UniqueDescriptorSet>
    {
        const std::vector<vk::DescriptorSetLayout> layouts {this->number_of_sets, this->layout};

        const vk::DescriptorSetAllocateInfo allocateInfo
        {
            .sType              {vk::StructureType::eDescriptorSetAllocateInfo},
            .pNext              {nullptr},
            .descriptorPool     {*this->pool},
            .descriptorSetCount {static_cast<std::uint32_t>(this->number_of_sets)},
            .pSetLayouts        {layouts.data()},
        };

        return this->device.allocateDescriptorSetsUnique(allocateInfo);
    }

} // namespace render