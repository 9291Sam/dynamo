#ifndef SRC_RENDER_VULKAN_PIPELINE_HPP
#define SRC_RENDER_VULKAN_PIPELINE_HPP

#include "includes.hpp"

namespace render
{
    class Pipeline
    {
    public:
        static vk::UniqueShaderModule createShaderFromFile(vk::Device, const std::string& filePath);
    public:
    
        Pipeline(vk::Device, vk::RenderPass, vk::Extent2D swapchainExtent,
            vk::UniqueShaderModule vertexShader, vk::UniqueShaderModule fragmentShader);
        ~Pipeline()                          = default;

        Pipeline()                           = delete;
        Pipeline(const Pipeline&)            = delete;
        Pipeline(Pipeline&&)                 = delete;
        Pipeline& operator=(const Pipeline&) = delete;
        Pipeline& operator=(Pipeline&&)      = delete; 

        [[nodiscard]] vk::Pipeline operator*() const;
        [[nodiscard]] vk::PipelineLayout getLayout() const;
        [[nodiscard]] vk::DescriptorSetLayout getDescriptorSetLayout() const;
            
    private:
        vk::UniqueDescriptorSetLayout descriptor_layout;
        vk::UniquePipelineLayout      layout;
        vk::UniquePipeline            pipeline;
    }; // class Pipeline
} // namespace render

#endif // SRC_RENDER_PIPELINE_HPP