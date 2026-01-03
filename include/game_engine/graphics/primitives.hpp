#pragma once

#include <game_engine/graphics/vertex_layout.hpp>
#include <vector>
#include <cstdint>

namespace game_engine::graphics {

    struct PrimitiveData {
        std::vector<VertexPositionNormalUV> vertices;
        std::vector<uint32_t> indices;
    };

    PrimitiveData generateCube(float size = 1.0f);
    PrimitiveData generateSphere(float radius = 0.5f, int rings = 16, int sectors = 32);
    PrimitiveData generatePlane(float width = 1.0f, float height = 1.0f);
    PrimitiveData generateQuad(float width = 1.0f, float height = 1.0f);

}
