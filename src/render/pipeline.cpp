#include <sebib/seblog.hpp>

#include "gpu_structs.hpp"

#include "pipeline.hpp"

namespace render
{

    Pipeline::Pipeline(vk::Device device, vk::RenderPass renderPass, vk::Extent2D swapchainExtent,
        vk::UniqueShaderModule&& vertexShader, vk::UniqueShaderModule&& fragmentShader)
    {
        vk::PipelineShaderStageCreateInfo vertexCreateInfo {
            .sType               {vk::StructureType::ePipelineShaderStageCreateInfo},
            .pNext               {nullptr},
            .flags               {},
            .stage               {vk::ShaderStageFlagBits::eVertex},
            .module              {*vertexShader},
            .pName               {"main"},
            .pSpecializationInfo {nullptr},
        };

        vk::PipelineShaderStageCreateInfo fragmentCreateInfo {
            .sType               {vk::StructureType::ePipelineShaderStageCreateInfo},
            .pNext               {nullptr},
            .flags               {},
            .stage               {vk::ShaderStageFlagBits::eFragment},
            .module              {*fragmentShader},
            .pName               {"main"},
            .pSpecializationInfo {nullptr},
        };

        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages {
            vertexCreateInfo,
            fragmentCreateInfo
        };

        vk::PipelineVertexInputStateCreateInfo pipeVertexCreateInfo {
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

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo {
            .sType                  {vk::StructureType::ePipelineInputAssemblyStateCreateInfo},
            .pNext                  {},
            .flags                  {},
            .topology               {vk::PrimitiveTopology::eTriangleList},
            .primitiveRestartEnable {false}, // enables 0xFFFFFFFF triangle strip restart
        };

        vk::Viewport viewport {
            .x        {0.0f},
            .y        {0.0f},
            .width    {static_cast<float>(swapchainExtent.width)},
            .height   {static_cast<float>(swapchainExtent.height)},
            .minDepth {0.0f},
            .maxDepth {1.0f},
        };

        vk::Rect2D scissor {
            .offset {0, 0},
            .extent {swapchainExtent}
        };

        vk::PipelineViewportStateCreateInfo viewportState {
            .sType         {vk::StructureType::ePipelineViewportStateCreateInfo},
            .pNext         {nullptr},
            .flags         {},
            .viewportCount {1},
            .pViewports    {&viewport},
            .scissorCount  {1},
            .pScissors     {&scissor},
        };

        vk::PipelineRasterizationStateCreateInfo rasterizationCreateInfo {
            .sType                   {vk::StructureType::ePipelineRasterizationStateCreateInfo},
            .pNext                   {nullptr},
            .flags                   {},
            .depthClampEnable        {false},
            .rasterizerDiscardEnable {false},
            .polygonMode             {vk::PolygonMode::eFill},
            .cullMode                {vk::CullModeFlagBits::eNone}, // PERF: change to back once refactor
            .frontFace               {vk::FrontFace::eCounterClockwise},
            .depthBiasEnable         {false},
            .depthBiasConstantFactor {0.0f},
            .depthBiasClamp          {0.0f},
            .depthBiasSlopeFactor    {0.0f},
            .lineWidth               {1.0f},
        };
        
        vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo {
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

        vk::PipelineColorBlendAttachmentState colorBlendAttachment {
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
        
        vk::PipelineColorBlendStateCreateInfo colorBlendCreateInfo {
            .sType           {vk::StructureType::ePipelineColorBlendStateCreateInfo},
            .pNext           {nullptr},
            .flags           {},
            .logicOpEnable   {false},
            .logicOp         {vk::LogicOp::eCopy},
            .attachmentCount {1},
            .pAttachments    {&colorBlendAttachment},
            .blendConstants  {{0.0f, 0.0f, 0.0f, 0.0f}},
        };

        vk::PushConstantRange pushConstantsInformation{
            .stageFlags {vk::ShaderStageFlagBits::eVertex},
            .offset     {0},
            .size       {sizeof(PushConstants)},
        };

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo {
            .sType                  {vk::StructureType::ePipelineLayoutCreateInfo},
            .pNext                  {nullptr},
            .flags                  {},
            .setLayoutCount         {0},
            .pSetLayouts            {nullptr},
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

} // namespace render