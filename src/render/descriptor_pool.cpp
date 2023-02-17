#include "descriptor_pool.hpp"

namespace render
{

    DescriptorPool::DescriptorPool(vk::Device device_, std::size_t numberOfSets)
        : device {device_}
        , number_of_sets {numberOfSets}
    {
        const vk::DescriptorPoolSize poolSize
        {
            .type            {vk::DescriptorType::eUniformBuffer},
            .descriptorCount {static_cast<std::uint32_t>(numberOfSets)}
        };

        const vk::DescriptorPoolCreateInfo poolCreateInfo
        {
            .sType         {vk::StructureType::eDescriptorPoolCreateInfo},
            .pNext         {nullptr},
            .flags         {},
            .maxSets       {static_cast<std::uint32_t>(numberOfSets)},
            .poolSizeCount {1},
            .pPoolSizes    {&poolSize}
        };

        this->pool = device.createDescriptorPoolUnique(poolCreateInfo);
    }

    auto DescriptorPool::allocate(vk::DescriptorSetLayout layout) const
        -> std::vector<vk::UniqueDescriptorSet>
    {
        const std::vector<vk::DescriptorSetLayout> layouts {this->number_of_sets, layout};

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