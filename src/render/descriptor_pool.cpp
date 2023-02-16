#include "descriptor_pool.hpp"

namespace render
{

    DescriptorPool::DescriptorPool(vk::Device device)
    {
        const vk::DescriptorSetLayoutBinding uniformBinding
        {
            .binding            {},
            .descriptorType     {vk::DescriptorType::eUniformBuffer},
            .descriptorCount    {1},
            .stageFlags         {vk::ShaderStageFlagBits::eAllGraphics},
            .pImmutableSamplers {nullptr},
        };
    }

} // namespace render