#pragma once

#include <cstdint>
#include <vector>

namespace game_engine::graphics {

    enum class VertexLayoutType : uint8_t {
        Position,
        PositionColor,
        PositionUV,
        PositionNormalUV,
    };

    enum class VertexFormat : uint32_t {
        Float32x2 = 0x00000003,
        Float32x3 = 0x00000004,
        Float32x4 = 0x00000005,
    };

    struct VertexAttribute {
        uint32_t location;
        uint32_t offset;
        VertexFormat format;
    };

    struct VertexLayout {
        std::vector<VertexAttribute> attributes;
        uint32_t stride;
    };

    VertexLayout getVertexLayout(VertexLayoutType type);

    struct VertexPosition {
        float x, y, z;
    };

    struct VertexPositionColor {
        float x, y, z;
        float r, g, b, a;
    };

    struct VertexPositionUV {
        float x, y, z;
        float u, v;
    };

    struct VertexPositionNormalUV {
        float x, y, z;
        float nx, ny, nz;
        float u, v;
    };

}
