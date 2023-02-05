#include "world.hpp"

static auto readVerticesFromFile(const std::string& filepath)
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


World::World()
{
    auto [v, i] = render::Object::readVerticesFromFile("../models/gizmo.obj");
    objects.push_back(renderer.createObject(std::move(v), std::move(i)));
    objects.at(0).transform.scale = {4.0f, 4.0f, 4.0f};

    auto [b, j] = render::Object::readVerticesFromFile("../models/floor.obj");
    objects.push_back(renderer.createObject(std::move(b), std::move(j)));
    objects.at(1).transform.scale = {100.0f, 100.0f, 100.0f};
    objects.at(1).transform.translation.y -= 10.0f;
}
std::vector<render::Object> objects;

        