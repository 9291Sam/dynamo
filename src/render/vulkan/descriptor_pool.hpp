#ifndef SRC_RENDER_VULKAN_DESCRIPTOR__POOL_HPP
#define SRC_RENDER_VULKAN_DESCRIPTOR__POOL_HPP

#include "includes.hpp"

namespace render
{

    class DescriptorPool
    {
    public:

        DescriptorPool(
            vk::Device, 
            vk::DescriptorSetLayout,
            const std::vector<vk::DescriptorPoolSize>&
        );
        ~DescriptorPool()                                = default;

        DescriptorPool()                                 = delete;
        DescriptorPool(const DescriptorPool&)            = delete;
        DescriptorPool(DescriptorPool&&)                 = default;
        DescriptorPool& operator=(const DescriptorPool&) = delete;
        DescriptorPool& operator=(DescriptorPool&&)      = default;

        [[nodiscard]] vk::DescriptorPool operator*() const;
        [[nodiscard]] std::vector<vk::UniqueDescriptorSet> allocate() const;
    
    private:
        vk::Device               device;
        vk::DescriptorSetLayout  layout;
        vk::UniqueDescriptorPool pool;
        std::size_t              number_of_sets;
    }; // class DescriptorPool

} // namespace render

#endif // SRC_RENDER_DESCRIPTOR__POOL_HPP