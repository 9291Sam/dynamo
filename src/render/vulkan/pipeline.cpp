#include <fstream>

#include <sebib/seblog.hpp>

#include "gpu_structs.hpp"

#include "pipeline.hpp"

static inline auto loadFileAsBytes(const std::string& filePath)
    -> std::vector<char>
{
    std::ifstream fileStream {filePath, std::ios::in | std::ios::binary | std::ios::ate};
    seb::assertFatal(fileStream.is_open(), "Failed to open file [{}]", filePath);

    std::vector<char> outputBuffer(static_cast<std::size_t>(fileStream.tellg()));

    fileStream.seekg(0); // Read from beginning  
    fileStream.read(outputBuffer.data(), outputBuffer.size());

    return outputBuffer;
}

static inline auto createShaderModuleFromSPIRV(
    vk::Device device, 
    const std::vector<char>& spirvBytes)
    -> vk::UniqueShaderModule
{
    vk::ShaderModuleCreateInfo createInfo {
        .sType    {vk::StructureType::eShaderModuleCreateInfo},
        .pNext    {nullptr},
        .flags    {},
        .codeSize {spirvBytes.    size()}, // std::vector's default allocator ensures this is fine
        .pCode    {reinterpret_cast<const uint32_t*>(spirvBytes.data())}, 
    };

    return device.createShaderModuleUnique(createInfo);
}

namespace render
{
    vk::UniqueShaderModule Pipeline::createShaderFromFile(vk::Device device, const std::string& filePath)
    {
        return createShaderModuleFromSPIRV(device, loadFileAsBytes(filePath));
    }    
    
    Pipeline::Pipeline(vk::Device device, vk::RenderPass renderPass, vk::Extent2D swapchainExtent,
        vk::UniqueShaderModule vertexShader, vk::UniqueShaderModule fragmentShader)
    {
        const vk::PipelineShaderStageCreateInfo vertexCreateInfo {
            .sType               {vk::StructureType::ePipelineShaderStageCreateInfo},
            .pNext               {nullptr},
            .flags               {},
            .stage               {vk::ShaderStageFlagBits::eVertex},
            .module              {*vertexShader},
            .pName               {"main"},
            .pSpecializationInfo {nullptr},
        };

        const vk::PipelineShaderStageCreateInfo fragmentCreateInfo {
            .sType               {vk::StructureType::ePipelineShaderStageCreateInfo},
            .pNext               {nullptr},
            .flags               {},
            .stage               {vk::ShaderStageFlagBits::eFragment},
            .module              {*fragmentShader},
            .pName               {"main"},
            .pSpecializationInfo {nullptr},
        };

        const std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages {
            vertexCreateInfo,
            fragmentCreateInfo
        };

        const vk::PipelineVertexInputStateCreateInfo pipeVertexCreateInfo {
            .sType                           {
                vk::StructureType::ePipelineVertexInputStateCreateInfo},
            .pNext                           {nullptr},
            .flags                           {},
            .vertexBindingDescriptionCount   {1},
            .pVertexBindingDescriptions      {Vertex::getBindingDescription()},
            .vertexAttributeDescriptionCount {
                static_cast<std::uint32_t>(Vertex::getAttributeDescriptions()->size())},
            .pVertexAttributeDescriptions    {Vertex::getAttributeDescriptions()->data()},
        };

        const vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo {
            .sType                  {vk::StructureType::ePipelineInputAssemblyStateCreateInfo},
            .pNext                  {},
            .flags                  {},
            .topology               {vk::PrimitiveTopology::eTriangleList},
            .primitiveRestartEnable {false}, // enables 0xFFFFFFFF triangle strip restart
        };

        const vk::Viewport viewport {
            .x        {0.0f},
            .y        {0.0f},
            .width    {static_cast<float>(swapchainExtent.width)},
            .height   {static_cast<float>(swapchainExtent.height)},
            .minDepth {0.0f},
            .maxDepth {1.0f},
        };

        const vk::Rect2D scissor {
            .offset {0, 0},
            .extent {swapchainExtent}
        };

        const vk::PipelineViewportStateCreateInfo viewportState {
            .sType         {vk::StructureType::ePipelineViewportStateCreateInfo},
            .pNext         {nullptr},
            .flags         {},
            .viewportCount {1},
            .pViewports    {&viewport},
            .scissorCount  {1},
            .pScissors     {&scissor},
        };

        const vk::PipelineRasterizationStateCreateInfo rasterizationCreateInfo {
            .sType                   {vk::StructureType::ePipelineRasterizationStateCreateInfo},
            .pNext                   {nullptr},
            .flags                   {},
            .depthClampEnable        {false},
            .rasterizerDiscardEnable {false},
            .polygonMode             {vk::PolygonMode::eFill},
            .cullMode                {vk::CullModeFlagBits::eBack}, // PERF: change to back once refactor
            .frontFace               {vk::FrontFace::eCounterClockwise},
            .depthBiasEnable         {false},
            .depthBiasConstantFactor {0.0f},
            .depthBiasClamp          {0.0f},
            .depthBiasSlopeFactor    {0.0f},
            .lineWidth               {1.0f},
        };
        
        const vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo {
            .sType                 {vk::StructureType::ePipelineMultisampleStateCreateInfo},
            .pNext                 {nullptr},
            .flags                 {},
            .rasterizationSamples  {vk::SampleCountFlagBits::e1},
            .sampleShadingEnable   {false},
            .minSampleShading      {1.0f},
            .pSampleMask           {nullptr},
            .alphaToCoverageEnable {false},
            .alphaToOneEnable      {false},
        };

        const vk::PipelineColorBlendAttachmentState colorBlendAttachment {
            .blendEnable         {false},
            .srcColorBlendFactor {VULKAN_HPP_NAMESPACE::BlendFactor::eZero},
            .dstColorBlendFactor {VULKAN_HPP_NAMESPACE::BlendFactor::eOne},
            .colorBlendOp        {VULKAN_HPP_NAMESPACE::BlendOp::eAdd},
            .srcAlphaBlendFactor {VULKAN_HPP_NAMESPACE::BlendFactor::eOne},
            .dstAlphaBlendFactor {VULKAN_HPP_NAMESPACE::BlendFactor::eZero},
            .alphaBlendOp        {VULKAN_HPP_NAMESPACE::BlendOp::eAdd},
            .colorWriteMask      {
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA 
            },
        };
        
        const vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo {
            .sType           {vk::StructureType::ePipelineColorBlendStateCreateInfo},
            .pNext           {nullptr},
            .flags           {},
            .logicOpEnable   {false},
            .logicOp         {vk::LogicOp::eCopy},
            .attachmentCount {1},
            .pAttachments    {&colorBlendAttachment},
            .blendConstants  {{0.0f, 0.0f, 0.0f, 0.0f}},
        };

        const vk::PushConstantRange pushConstantsInformation{
            .stageFlags {vk::ShaderStageFlagBits::eAllGraphics},
            .offset     {0},
            .size       {sizeof(PushConstants)},
        };

        const std::array<vk::DescriptorSetLayoutBinding, 2> descriptorSetBindings
        {
            vk::DescriptorSetLayoutBinding
            {
                .binding            {0},
                .descriptorType     {vk::DescriptorType::eUniformBuffer},
                .descriptorCount    {1},
                .stageFlags         {vk::ShaderStageFlagBits::eAllGraphics},
                .pImmutableSamplers {nullptr},
            },
            vk::DescriptorSetLayoutBinding
            {
                .binding            {1},
                .descriptorType     {vk::DescriptorType::eCombinedImageSampler},
                .descriptorCount    {1},
                .stageFlags         {vk::ShaderStageFlagBits::eFragment},
                .pImmutableSamplers {nullptr},
            }
        };
        

        const vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo
        {
            .sType        {vk::StructureType::eDescriptorSetLayoutCreateInfo},
            .pNext        {nullptr},
            .flags        {},
            .bindingCount {descriptorSetBindings.size()},
            .pBindings    {descriptorSetBindings.data()},
        };

        this->descriptor_layout = device.createDescriptorSetLayoutUnique(descriptorSetLayoutInfo);

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
            .sType                  {vk::StructureType::ePipelineLayoutCreateInfo},
            .pNext                  {nullptr},
            .flags                  {},
            .setLayoutCount         {1},
            .pSetLayouts            {&*this->descriptor_layout},
            .pushConstantRangeCount {1},
            .pPushConstantRanges    {&pushConstantsInformation},
        };

        this->layout = device.createPipelineLayoutUnique(pipelineLayoutInfo);

        vk::PipelineDepthStencilStateCreateInfo depthStencilActivator {
            .sType                 {vk::StructureType::ePipelineDepthStencilStateCreateInfo},
            .pNext                 {nullptr},
            .flags                 {},
            .depthTestEnable       {true},
            .depthWriteEnable      {true},
            .depthCompareOp        {vk::CompareOp::eLess},
            .depthBoundsTestEnable {false},
            .stencilTestEnable     {false},
            .front                 {},
            .back                  {},
            .minDepthBounds        {0.0f},
            .maxDepthBounds        {1.0f},
        };



        vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo {
            .sType               {vk::StructureType::eGraphicsPipelineCreateInfo},
            .pNext               {nullptr},
            .flags               {},
            .stageCount          {shaderStages.size()},
            .pStages             {shaderStages.data()},
            .pVertexInputState   {&pipeVertexCreateInfo},
            .pInputAssemblyState {&inputAssemblyCreateInfo},
            .pTessellationState  {nullptr},
            .pViewportState      {&viewportState},
            .pRasterizationState {&rasterizationCreateInfo},
            .pMultisampleState   {&multiSampleStateCreateInfo},
            .pDepthStencilState  {&depthStencilActivator},
            .pColorBlendState    {&colorBlendCreateInfo},
            .pDynamicState       {nullptr},
            .layout              {*this->layout},
            .renderPass          {renderPass},
            .subpass             {0},
            .basePipelineHandle  {nullptr},
            .basePipelineIndex   {-1},
        };

        auto [result, maybeGraphicsPipeline] = device.
            createGraphicsPipelineUnique(
                nullptr, graphicsPipelineCreateInfo);

        seb::assertFatal(
            result == vk::Result::eSuccess,
            "Failed to create graphics pipeline"
        );

        this->pipeline = std::move(maybeGraphicsPipeline);
    }
    
    vk::Pipeline Pipeline::operator*() const
    {
        return *this->pipeline;
    }

    vk::PipelineLayout Pipeline::getLayout() const
    {
        return *this->layout;
    }

    vk::DescriptorSetLayout Pipeline::getDescriptorSetLayout() const
    {
        return *this->descriptor_layout;
    }

} // namespace render