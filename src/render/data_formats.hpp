#pragma once

#ifndef SRC_RENDER_VERTEX_HPP
#define SRC_RENDER_VERTEX_HPP

#include "vulkan_includes.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>                 
#include <glm/vec4.hpp>                 
#include <glm/mat4x4.hpp>               
#include <glm/mat3x3.hpp>  
#include <glm/ext/matrix_transform.hpp> 
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp> 
#include <glm/gtc/quaternion.hpp> 
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

namespace render
{
    /// NOTE:
    /// If you change any of these, dont forget to update their corresponding
    /// structs in the shaders!

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 color; // TODO: remove
        glm::vec3 normal;
        glm::vec2 uv;

        [[nodiscard, gnu::const]] static auto getBindingDescription()
            -> const vk::VertexInputBindingDescription*;
        
        [[nodiscard, gnu::const]] static auto getAttributeDescriptions()
            -> const std::array<vk::VertexInputAttributeDescription, 4>*;

        [[nodiscard]] operator std::string() const;
    };

    struct PushConstants
    {
        glm::mat4 model_view_projection;
    };

    struct UniformBuffer
    {

    };
} // namespace render

#endif // SRC_RENDER_VERTEX_HPP