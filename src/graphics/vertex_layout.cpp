#include <game_engine/graphics/vertex_layout.hpp>

namespace game_engine::graphics {

    VertexLayout getVertexLayout(VertexLayoutType type) {
        VertexLayout layout;

        switch (type) {
            case VertexLayoutType::Position:
                layout.stride = sizeof(VertexPosition);
                layout.attributes = {
                    {0, 0, VertexFormat::Float32x3}
                };
                break;

            case VertexLayoutType::PositionColor:
                layout.stride = sizeof(VertexPositionColor);
                layout.attributes = {
                    {0, 0, VertexFormat::Float32x3},
                    {1, sizeof(float) * 3, VertexFormat::Float32x4}
                };
                break;

            case VertexLayoutType::PositionUV:
                layout.stride = sizeof(VertexPositionUV);
                layout.attributes = {
                    {0, 0, VertexFormat::Float32x3},
                    {1, sizeof(float) * 3, VertexFormat::Float32x2}
                };
                break;

            case VertexLayoutType::PositionNormalUV:
                layout.stride = sizeof(VertexPositionNormalUV);
                layout.attributes = {
                    {0, 0, VertexFormat::Float32x3},
                    {1, sizeof(float) * 3, VertexFormat::Float32x3},
                    {2, sizeof(float) * 6, VertexFormat::Float32x2}
                };
                break;
        }

        return layout;
    }

}
