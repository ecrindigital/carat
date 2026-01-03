#pragma once

#include <game_engine/graphics/vertex_layout.hpp>
#include <cstdint>
#include <string_view>

namespace game_engine::graphics {

    enum class BuiltinShader : uint8_t {
        Unlit,
        UnlitTextured,
        Sprite,
        Lit,
        PBR
    };

    struct ShaderDefinition {
        std::string_view vertexCode;
        std::string_view fragmentCode;
        VertexLayoutType vertexLayout;
        bool hasUniformBuffer;
        bool hasTexture;
    };

    const ShaderDefinition& getShaderDefinition(BuiltinShader shader);

} // namespace game_engine::graphics
