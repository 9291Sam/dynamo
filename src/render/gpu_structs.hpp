#pragma once

#ifndef SRC_RENDER_GPU__STRUCTS_HPP
#define SRC_RENDER_GPU__STRUCTS_HPP

#include "vulkan_includes.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsuggest-attribute=format"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
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
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

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
        [[nodiscard]] bool operator==(const Vertex&) const = default;
    };

    using Index = std::uint32_t;

    struct PushConstants
    {
        glm::mat4 model_view_projection;
        glm::mat3 normal_matrix;
    };

    struct UniformBuffer
    {
        std::uint32_t numberOfLights;
        std::array<std::uint32_t, 3> _padding;
        std::array<glm::vec4, 32> lights;
    };
} // namespace render

// Vertex hash implementation
namespace std
{
    template<>
    struct hash<render::Vertex>
    {
        [[nodiscard]] auto operator()(const render::Vertex& vertex)
            const noexcept -> size_t
        {
            std::size_t seed {0};
            std::hash<float> hasher;
            
            auto hashCombine = [](std::size_t& seed_, std::size_t hash_)
            {
                hash_ += 0x9e3779b9 + (seed_ << 6) + (seed_ >> 2);
                seed_ ^= hash_;
            };

            hashCombine(seed, hasher(vertex.position.x));
            hashCombine(seed, hasher(vertex.position.y));
            hashCombine(seed, hasher(vertex.position.z));

            hashCombine(seed, hasher(vertex.color.x));
            hashCombine(seed, hasher(vertex.color.y));
            hashCombine(seed, hasher(vertex.color.z));

            hashCombine(seed, hasher(vertex.normal.x));
            hashCombine(seed, hasher(vertex.normal.y));
            hashCombine(seed, hasher(vertex.normal.z));

            hashCombine(seed, hasher(vertex.uv.x));
            hashCombine(seed, hasher(vertex.uv.y));

            return seed;
        }
    };
    
} // namespace std

#endif // SRC_RENDER_GPU__STRUCTS_HPP