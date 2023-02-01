#include <fmt/format.h>

#include "data_formats.hpp"

namespace render
{
    auto Vertex::getBindingDescription()
        -> const vk::VertexInputBindingDescription* 
    {
        static const vk::VertexInputBindingDescription bindings{
            .binding   {0},
            .stride    {sizeof(Vertex)},
            .inputRate {vk::VertexInputRate::eVertex},
        };
        return &bindings;
    }

    auto Vertex::getAttributeDescriptions()
        -> const std::array<vk::VertexInputAttributeDescription, 4>*
    {
        static const std::array<vk::VertexInputAttributeDescription, 4> descriptions {
            vk::VertexInputAttributeDescription {
                .location {0},
                .binding  {0},
                .format   {vk::Format::eR32G32B32Sfloat},
                .offset   {offsetof(Vertex, position)},
            },
            vk::VertexInputAttributeDescription {
                .location {1},
                .binding  {0},
                .format   {vk::Format::eR32G32B32Sfloat},
                .offset   {offsetof(Vertex, color)},
            },
            vk::VertexInputAttributeDescription {
                .location {2},
                .binding  {0},
                .format   {vk::Format::eR32G32B32Sfloat},
                .offset   {offsetof(Vertex, normal)},
            },
            vk::VertexInputAttributeDescription {
                .location {3},
                .binding  {0},
                .format   {vk::Format::eR32G32Sfloat},
                .offset   {offsetof(Vertex, uv)},
            },
        }; 
        return &descriptions;
    }

    [[nodiscard]] Vertex::operator std::string() const
    {
        return fmt::format("Location: X: {}, Y: {}, Z: {} | Color: R: {}, G: {}, B: {} | Normal: X: {}, Y: {}, Z: {} | UV: U: {}, V: {}",
            this->position.x,
            this->position.y,
            this->position.z,
            this->color.r,
            this->color.g,
            this->color.b,
            this->normal.x,
            this->normal.y, 
            this->normal.z,
            this->uv.x,
            this->uv.y
        );
    }
}