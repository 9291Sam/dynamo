#pragma once

#ifndef SRC_RENDER_DESCRIPTOR__POOL_HPP
#define SRC_RENDER_DESCRIPTOR__POOL_HPP

#include "gpu_structs.hpp"

namespace render
{

    class DescriptorPool
    {
    public:

        DescriptorPool(vk::Device);
        ~DescriptorPool() = default;

        // DescriptorPool()                                 = delete;
        DescriptorPool(const DescriptorPool&)            = delete;
        DescriptorPool(DescriptorPool&&)                 = delete;
        DescriptorPool& operator=(const DescriptorPool&) = delete;
        DescriptorPool& operator=(DescriptorPool&&)      = delete;
    
    private:

        // vk::DescriptorSetLayoutBinding uniform_binding;

    }; // class DescriptorPool

} // namespace render

#endif // SRC_RENDER_DESCRIPTOR__POOL_HPP