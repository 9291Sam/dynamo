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

        [[nodiscard, gnu::pure]] glm::vec3 getForwardVector() const;
        [[nodiscard, gnu::pure]] glm::vec3 getUpVector() const;
        [[nodiscard, gnu::pure]] glm::vec3 getRightVector() const;

        [[nodiscard]] explicit operator std::string() const;
    };

    // TODO: figure out how to stage if persistent
    // acrtually a good use of an abstract class?
    class Object
    {
    public:
        static auto readVerticesFromFile(const std::string& filepath)
            -> std::pair<std::vector<render::Vertex>, std::vector<uint32_t>>;
            
        Object(VmaAllocator, std::vector<Vertex>, std::optional<std::vector<Index>>);
        ~Object()                        = default;

        Object()                         = delete;
        Object(const Object&)            = delete;
        Object(Object&&)                 = default;
        Object& operator=(const Object&) = delete; 
        Object& operator=(Object&&)      = default;

        void bind(vk::CommandBuffer) const;
        void draw(vk::CommandBuffer) const;

        Transform transform;

    private:
        
        std::vector<Vertex> vertices;
        std::optional<std::vector<Index>> indicies;

        Buffer vertex_buffer;   
        std::optional<Buffer> index_buffer;

    }; // class Object

    // TODO: This is quite a bad stateful design, try and fix this.
    class Camera
    {
    public:

        Camera(const glm::vec3& position, float pitch_, float yaw_);

        void addPosition(const glm::vec3& positionToAdd);

        void update(std::function<bool(vkfw::Key)>, std::pair<double, double> mouseDeltas, float deltaTime);

        void addPitch(float pitchToAdd);
        void addYaw(float yawToAdd);

        [[nodiscard, gnu::pure]] auto getForwardVector()
            -> glm::vec3;
        [[nodiscard, gnu::pure]] auto getRightVector()
            -> glm::vec3;
        [[nodiscard, gnu::pure]] auto getUpVector()
            -> glm::vec3;

        [[nodiscard]] explicit operator std::string() const;

        auto asViewMatrix() const
            -> glm::mat4;
        
        static auto getPerspectiveMatrix(float fovY, float aspectRatio,
            float closeClippingPlaneDistance, float farClippingPlaneDistance)
            -> glm::mat4;

    private:

        void updateRotation();

        Transform transform;
        float pitch;
        float yaw;

    };

} // namespace render

#endif // SRC_RENDER_OBJECT_HPP

/// one vulkan object
/// can be bound and then drawn with a draw command