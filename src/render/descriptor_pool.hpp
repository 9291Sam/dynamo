#pragma once

#ifndef SRC_RENDER_DESCRIPTOR__POOL_HPP
#define SRC_RENDER_DESCRIPTOR__POOL_HPP

#include "gpu_structs.hpp"

namespace render
{

    class DescriptorPool
    {
    public:

        DescriptorPool(vk::Device, std::size_t numberOfSets);
        ~DescriptorPool()                                = default;

        DescriptorPool()                                 = delete;
        DescriptorPool(const DescriptorPool&)            = delete;
        DescriptorPool(DescriptorPool&&)                 = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;
        DescriptorPool& operator=(DescriptorPool&&)      = delete;

        [[nodiscard, gnu::pure]] vk::DescriptorPool operator*() const;
        [[nodiscard, gnu::pure]] auto allocate(vk::DescriptorSetLayout) const 
            -> std::vector<vk::UniqueDescriptorSet>;
    
    private:
        vk::Device device;
        vk::UniqueDescriptorPool pool;
        std::size_t number_of_sets;

    }; // class DescriptorPool

} // namespace render

#endif // SRC_RENDER_DESCRIPTOR__POOL_HPP