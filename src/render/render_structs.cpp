#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#pragma GCC diagnostic pop

#include <fmt/format.h>

#include <sebib/seblog.hpp>

#include "render_structs.hpp"



namespace render
{
    Transform::Transform()
        : translation {0.0f, 0.0f, 0.0f}
        , rotation {1.0f, 0.0f, 0.0f, 0.0f}
        , scale {1.0f, 1.0f, 1.0f}
    {}

    glm::mat4 Transform::asModelMatrix() const
    {
        return this->asTranslationMatrix() * this->asRotationMatrix() * this->asScaleMatrix();
    }

    glm::mat4 Transform::asTranslationMatrix() const
    {
        return glm::translate(glm::mat4{1.0f}, this->translation);
    }

    glm::mat4 Transform::asRotationMatrix() const
    {
        return glm::mat4_cast(this->rotation);
    }

    glm::mat4 Transform::asScaleMatrix() const
    {
        return glm::scale(glm::mat4{1.0f}, this->scale);
    }

    glm::vec3 Transform::getForwardVector() const
    {
        return this->rotation * Transform::ForwardVector;
    }

    glm::vec3 Transform::getUpVector() const
    {
        return this->rotation * Transform::UpVector;
    }

    glm::vec3 Transform::getRightVector() const
    {
        return this->rotation * Transform::RightVector; 
    }

    Transform::operator std::string() const
    {
        return fmt::format("Transform: Pos: {} | Rot: {} | Scale: {}",
            glm::to_string(this->translation),
            glm::to_string(this->rotation),
            glm::to_string(this->scale)
        );
    }

    auto Object::readVerticesFromFile(const std::string& filepath)
        -> std::pair<std::vector<render::Vertex>, std::vector<uint32_t>>
    {
        tinyobj::attrib_t attribute {}; 
        std::vector<tinyobj::shape_t> shapes; 
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string error;

        seb::assertFatal(
            tinyobj::LoadObj(&attribute, &shapes, &materials, &warn, &error, filepath.c_str()),
            "Failed to load file {} | Warn: {} | Error: {}",
            filepath, 
            warn,
            error
        );

        std::vector<Vertex> vertices;
        std::vector<std::uint32_t> indices;

        std::unordered_map<Vertex, std::size_t> uniqueVertices;

        for (const tinyobj::shape_t& shape : shapes)
        {
            for (const tinyobj::index_t& index : shape.mesh.indices)
            {
                Vertex vertex {
                    .position {
                        index.vertex_index >= 0 ?
                        glm::vec3 {
                            attribute.vertices.at(3 * index.vertex_index + 0),
                            attribute.vertices.at(3 * index.vertex_index + 1),
                            attribute.vertices.at(3 * index.vertex_index + 2),
                        } :
                        glm::vec3 {0.0f, 0.0f, 0.0f}
                    },
                    .color {
                        index.vertex_index >= 0 ? 
                        glm::vec3 {
                            attribute.colors.at(3 * index.vertex_index + 0),
                            attribute.colors.at(3 * index.vertex_index + 1),
                            attribute.colors.at(3 * index.vertex_index + 2),
                        } :
                        glm::vec3 {1.0f, 1.0f, 1.0f}
                    },
                    .normal {
                        index.normal_index >= 0 ?
                        glm::vec3 {
                            attribute.normals.at(3 * index.normal_index + 0),
                            attribute.normals.at(3 * index.normal_index + 1),
                            attribute.normals.at(3 * index.normal_index + 2),
                        } :
                        glm::vec3 {0.0f, 0.0f, 0.0f}
                    },
                    .uv {
                        index.texcoord_index >= 0 ?
                        glm::vec2 {
                            attribute.texcoords.at(2 * index.texcoord_index + 0),
                            attribute.texcoords.at(2 * index.texcoord_index + 1),
                        } :
                        glm::vec2 {0.0f, 0.0f}
                    }
                };

                // If this vertex is being encountered for the first time.
                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = vertices.size();

                    

                    vertices.push_back(vertex);
                }

                seb::assertWarn(vertices.size() < std::numeric_limits<std::uint32_t>::max(),
                        "Tried to load a model with too many vertices!");

                indices.push_back(static_cast<std::uint32_t>(uniqueVertices[vertex]));
            }
        }
    
        return std::make_pair(vertices, indices);
    }

    Object::Object(VmaAllocator allocator, std::vector<Vertex> vertices_,
        std::optional<std::vector<Index>> maybeIndicies)
        : vertices {std::move(vertices_)}
        , indicies {std::move(maybeIndicies)}
        , vertex_buffer {
            allocator,
            this->vertices.size() * sizeof(Vertex),
            vk::BufferUsageFlagBits::eVertexBuffer, 
            vk::MemoryPropertyFlagBits::eDeviceLocal | 
            vk::MemoryPropertyFlagBits::eHostVisible |
            vk::MemoryPropertyFlagBits::eHostCoherent
        }
        , index_buffer {
            this->indicies.has_value() ?
            std::make_optional<Buffer>(
                allocator,
                this->indicies->size() * sizeof(Index),
                vk::BufferUsageFlagBits::eIndexBuffer,
                vk::MemoryPropertyFlagBits::eDeviceLocal | 
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            )
            :
            std::nullopt
        }
    {
        this->vertex_buffer.write(
            std::span<const std::byte> {
                reinterpret_cast<const std::byte*>(vertices.data()),
                vertices.size() * sizeof (Vertex)
            }
        );

        if (this->index_buffer.has_value())
        {
            this->index_buffer->write(
                std::span<const std::byte> {
                    reinterpret_cast<const std::byte*>(this->indicies->data()),
                    this->indicies->size() * sizeof(Index) // size bytes
                }
            );
        }   
        
    }

    void Object::bind(vk::CommandBuffer commandBuffer) const
    {
        commandBuffer.bindVertexBuffers(
            0, 
            *this->vertex_buffer, 
            std::array<vk::DeviceSize, 1> {0}
        );

        if (this->index_buffer.has_value())
        {
            static_assert(sizeof(Index) == sizeof(std::uint32_t));
            commandBuffer.bindIndexBuffer(**this->index_buffer, 0, vk::IndexType::eUint32);
        }
    }

    void Object::draw(vk::CommandBuffer commandBuffer) const
    {
        if (this->index_buffer.has_value())
        {
            commandBuffer.drawIndexed(
                static_cast<std::uint32_t>(this->indicies->size()), 
                1, 0, 0, 0
            );
        }
        else
        {
            commandBuffer.draw(static_cast<std::uint32_t>(this->vertices.size()), 1, 0, 0);
        }
    }

    Camera::Camera(const glm::vec3& position, float pitch_, float yaw_) 
        : transform {}
        , pitch {pitch_}
        , yaw {yaw_}
    {
        this->transform.translation = position;
        this->updateRotation();
    }

    void Camera::addPosition(const glm::vec3& positionToAdd)
    {
        this->transform.translation += positionToAdd;
    }

    void Camera::updateFromKeys(std::function<bool(vkfw::Key)> keyPollCallback, float deltaTime)
    {
        const float moveSpeedScale = keyPollCallback(vkfw::Key::eLeftShift) ? 35.0f : 15.0f;
        const float rotateSpeedScale = 5.0f;

        if (keyPollCallback(vkfw::Key::eW))
        {
            this->addPosition(this->getForwardVector() * deltaTime * moveSpeedScale);
        }

        if (keyPollCallback(vkfw::Key::eS))
        {
            this->addPosition(this->getForwardVector() * deltaTime * moveSpeedScale * -1.0f);
        }

        if (keyPollCallback(vkfw::Key::eD))
        {
            this->addPosition(this->getRightVector() * deltaTime * moveSpeedScale);
        }

        if (keyPollCallback(vkfw::Key::eA))
        {
            this->addPosition(this->getRightVector() * deltaTime * moveSpeedScale * -1.0f);
        }

        if (keyPollCallback(vkfw::Key::eE))
        {
            this->addPosition(render::Transform::UpVector * deltaTime * moveSpeedScale * -1.0f);
        }

        if (keyPollCallback(vkfw::Key::eQ))
        {
            this->addPosition(render::Transform::UpVector * deltaTime * moveSpeedScale);
        }



        if (keyPollCallback(vkfw::Key::eLeft))
        {
            this->addYaw(deltaTime * rotateSpeedScale * -1.0f);
        }

        if (keyPollCallback(vkfw::Key::eRight))
        {
            this->addYaw(deltaTime * rotateSpeedScale);
        }

        if (keyPollCallback(vkfw::Key::eUp))
        {
            this->addPitch(deltaTime * rotateSpeedScale);
        }

        if (keyPollCallback(vkfw::Key::eDown))
        {
            this->addPitch(deltaTime * rotateSpeedScale * -1.0f);
        }
    }

    void Camera::addPitch(float pitchToAdd)
    {
        this->pitch += pitchToAdd;
        this->updateRotation();
    }

    void Camera::addYaw(float yawToAdd)
    {
        this->yaw += yawToAdd;
        this->updateRotation();
    }

    glm::vec3 Camera::getForwardVector()
    {
        return this->transform.getForwardVector();
    }

    glm::vec3 Camera::getRightVector()
    {
        return this->transform.getRightVector();
    }

    glm::vec3 Camera::getUpVector()
    {
        return this->transform.getUpVector();
    }

    Camera::operator std::string() const
    {
        return fmt::format("{} | Pitch {} | Yaw {}",
            static_cast<std::string>(this->transform), this->pitch, this->yaw);
    }

    auto Camera::asViewMatrix() const
        -> glm::mat4
    {
        return glm::inverse(
            this->transform.asTranslationMatrix() * 
            this->transform.asRotationMatrix()
        );
    }

    auto Camera::getPerspectiveMatrix(float fovY, float aspectRatio,
        float closeClippingPlaneDistance, float farClippingPlaneDistance)
        -> glm::mat4
    {
        glm::mat4 projection = glm::perspective(
            fovY, 
            aspectRatio, 
            closeClippingPlaneDistance, 
            farClippingPlaneDistance
        );
        projection[1][1] *= -1;

        return projection;
    }

    void Camera::updateRotation()
    {
        glm::quat q {1.0f, 0.0f, 0.0f, 0.0f};

        this->pitch = glm::mod(this->pitch, glm::two_pi<float>());
        this->yaw = glm::mod(this->yaw, glm::two_pi<float>());

        q *= glm::angleAxis(this->pitch, glm::vec3 {1.0f, 0.0f, 0.0f});

        q = glm::angleAxis(this->yaw, glm::vec3 {0.0f, -1.0f, 0.0f}) * q;

        this->transform.rotation = q;
            
    }
}