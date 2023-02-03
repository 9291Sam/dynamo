#pragma once

#ifndef SRC_RENDER_OBJECT_HPP
#define SRC_RENDER_OBJECT_HPP

#include <optional>
#include <vector>

#include "buffer.hpp"
#include "gpu_structs.hpp"

namespace render
{
    struct Transform
    {
        constexpr static glm::vec3 ForwardVector = glm::vec3 {0.0f, 0.0f, -1.0f};
        constexpr static glm::vec3 RightVector = glm::vec3 {1.0f, 0.0f, 0.0f};
        constexpr static glm::vec3 UpVector = glm::vec3 {0.0f, -1.0f, 0.0f};

        glm::vec3 translation;
        glm::quat rotation;
        glm::vec3 scale;

        Transform();

        [[nodiscard]] glm::mat4 asModelMatrix() const;
        [[nodiscard]] glm::mat4 asTranslationMatrix() const;
        [[nodiscard]] glm::mat4 asRotationMatrix() const;
        [[nodiscard]] glm::mat4 asScaleMatrix() const;

        [[nodiscard]] glm::vec3 getForwardVector() const;
        [[nodiscard]] glm::vec3 getUpVector() const;
        [[nodiscard]] glm::vec3 getRightVector() const;

        [[nodiscard]] explicit operator std::string() const;
    };

    // TODO: figure out how to stage if persistent
    class Object
    {
    public:
        Object(vk::Device, std::vector<Vertex>, std::optional<std::vector<Index>>);
        ~Object()                        = default;

        Object()                         = delete;
        Object(const Object&)            = delete;
        Object(Object&&)                 = default;
        Object& operator=(const Object&) = delete; 
        Object& operator=(Object&&)      = default;

        static void bind(std::span<Object>, vk::CommandBuffer);
        void bind(vk::CommandBuffer) const;
        void draw(vk::CommandBuffer) const;

        const Transform& getTransform() const;

    private:
        Transform transform;
        
        std::vector<Vertex> vertices;
        std::optional<std::vector<Index>> indicies;

        Buffer vertex_buffer;   
        std::optional<Buffer> index_buffer;

    }; // class Object

    class Camera
    {

    }; 

} // namespace render

#endif // SRC_RENDER_OBJECT_HPP

/// one vulkan object
/// can be bound and then drawn with a draw command