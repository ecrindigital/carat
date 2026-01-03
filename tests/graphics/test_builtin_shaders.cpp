#include <catch2/catch_all.hpp>
#include <game_engine/graphics/builtin_shaders.hpp>

using namespace game_engine::graphics;

TEST_CASE("BuiltinShader enum values") {
    SECTION("given Unlit shader when cast to int then returns 0") {
        REQUIRE(static_cast<int>(BuiltinShader::Unlit) == 0);
    }

    SECTION("given UnlitTextured shader when cast to int then returns 1") {
        REQUIRE(static_cast<int>(BuiltinShader::UnlitTextured) == 1);
    }

    SECTION("given Sprite shader when cast to int then returns 2") {
        REQUIRE(static_cast<int>(BuiltinShader::Sprite) == 2);
    }

    SECTION("given Lit shader when cast to int then returns 3") {
        REQUIRE(static_cast<int>(BuiltinShader::Lit) == 3);
    }

    SECTION("given PBR shader when cast to int then returns 4") {
        REQUIRE(static_cast<int>(BuiltinShader::PBR) == 4);
    }

    SECTION("given Glass shader when cast to int then returns 5") {
        REQUIRE(static_cast<int>(BuiltinShader::Glass) == 5);
    }

    SECTION("given Glow shader when cast to int then returns 6") {
        REQUIRE(static_cast<int>(BuiltinShader::Glow) == 6);
    }
}

TEST_CASE("ShaderDefinition structure") {
    SECTION("given Unlit shader when getting definition then has vertex code") {
        const auto& def = getShaderDefinition(BuiltinShader::Unlit);
        REQUIRE_FALSE(def.vertexCode.empty());
    }

    SECTION("given Unlit shader when getting definition then has fragment code") {
        const auto& def = getShaderDefinition(BuiltinShader::Unlit);
        REQUIRE_FALSE(def.fragmentCode.empty());
    }

    SECTION("given Unlit shader when getting definition then has PositionNormalUV layout") {
        const auto& def = getShaderDefinition(BuiltinShader::Unlit);
        REQUIRE(def.vertexLayout == VertexLayoutType::PositionNormalUV);
    }

    SECTION("given Unlit shader when getting definition then has uniform buffer") {
        const auto& def = getShaderDefinition(BuiltinShader::Unlit);
        REQUIRE(def.hasUniformBuffer == true);
    }

    SECTION("given Unlit shader when getting definition then has no texture") {
        const auto& def = getShaderDefinition(BuiltinShader::Unlit);
        REQUIRE(def.hasTexture == false);
    }
}

TEST_CASE("ShaderDefinition UnlitTextured") {
    SECTION("given UnlitTextured shader when getting definition then has texture") {
        const auto& def = getShaderDefinition(BuiltinShader::UnlitTextured);
        REQUIRE(def.hasTexture == true);
    }

    SECTION("given UnlitTextured shader when getting definition then has uniform buffer") {
        const auto& def = getShaderDefinition(BuiltinShader::UnlitTextured);
        REQUIRE(def.hasUniformBuffer == true);
    }

    SECTION("given UnlitTextured shader when getting definition then vertex code contains uv") {
        const auto& def = getShaderDefinition(BuiltinShader::UnlitTextured);
        REQUIRE(def.vertexCode.find("uv") != std::string_view::npos);
    }
}

TEST_CASE("ShaderDefinition Sprite") {
    SECTION("given Sprite shader when getting definition then has texture") {
        const auto& def = getShaderDefinition(BuiltinShader::Sprite);
        REQUIRE(def.hasTexture == true);
    }

    SECTION("given Sprite shader when getting definition then fragment contains discard") {
        const auto& def = getShaderDefinition(BuiltinShader::Sprite);
        REQUIRE(def.fragmentCode.find("discard") != std::string_view::npos);
    }

    SECTION("given Sprite shader when getting definition then has PositionNormalUV layout") {
        const auto& def = getShaderDefinition(BuiltinShader::Sprite);
        REQUIRE(def.vertexLayout == VertexLayoutType::PositionNormalUV);
    }
}

TEST_CASE("ShaderDefinition PBR") {
    SECTION("given PBR shader when getting definition then has no texture") {
        const auto& def = getShaderDefinition(BuiltinShader::PBR);
        REQUIRE(def.hasTexture == false);
    }

    SECTION("given PBR shader when getting definition then fragment contains distribution_ggx") {
        const auto& def = getShaderDefinition(BuiltinShader::PBR);
        REQUIRE(def.fragmentCode.find("distribution_ggx") != std::string_view::npos);
    }

    SECTION("given PBR shader when getting definition then fragment contains fresnel_schlick") {
        const auto& def = getShaderDefinition(BuiltinShader::PBR);
        REQUIRE(def.fragmentCode.find("fresnel_schlick") != std::string_view::npos);
    }

    SECTION("given PBR shader when getting definition then fragment contains geometry_smith") {
        const auto& def = getShaderDefinition(BuiltinShader::PBR);
        REQUIRE(def.fragmentCode.find("geometry_smith") != std::string_view::npos);
    }

    SECTION("given PBR shader when getting definition then fragment contains PBRMaterial struct") {
        const auto& def = getShaderDefinition(BuiltinShader::PBR);
        REQUIRE(def.fragmentCode.find("PBRMaterial") != std::string_view::npos);
    }

    SECTION("given PBR shader when getting definition then vertex outputs world_position") {
        const auto& def = getShaderDefinition(BuiltinShader::PBR);
        REQUIRE(def.vertexCode.find("world_position") != std::string_view::npos);
    }

    SECTION("given PBR shader when getting definition then vertex outputs world_normal") {
        const auto& def = getShaderDefinition(BuiltinShader::PBR);
        REQUIRE(def.vertexCode.find("world_normal") != std::string_view::npos);
    }
}

TEST_CASE("ShaderDefinition Glass") {
    SECTION("given Glass shader when getting definition then has no texture") {
        const auto& def = getShaderDefinition(BuiltinShader::Glass);
        REQUIRE(def.hasTexture == false);
    }

    SECTION("given Glass shader when getting definition then fragment contains GlassMaterial struct") {
        const auto& def = getShaderDefinition(BuiltinShader::Glass);
        REQUIRE(def.fragmentCode.find("GlassMaterial") != std::string_view::npos);
    }

    SECTION("given Glass shader when getting definition then fragment contains fresnel calculation") {
        const auto& def = getShaderDefinition(BuiltinShader::Glass);
        REQUIRE(def.fragmentCode.find("fresnel_schlick") != std::string_view::npos);
    }

    SECTION("given Glass shader when getting definition then fragment contains reflection") {
        const auto& def = getShaderDefinition(BuiltinShader::Glass);
        REQUIRE(def.fragmentCode.find("reflect") != std::string_view::npos);
    }

    SECTION("given Glass shader when getting definition then fragment handles lighting") {
        const auto& def = getShaderDefinition(BuiltinShader::Glass);
        REQUIRE(def.fragmentCode.find("LightingData") != std::string_view::npos);
    }
}

TEST_CASE("ShaderDefinition Glow") {
    SECTION("given Glow shader when getting definition then has no texture") {
        const auto& def = getShaderDefinition(BuiltinShader::Glow);
        REQUIRE(def.hasTexture == false);
    }

    SECTION("given Glow shader when getting definition then fragment contains smoothstep") {
        const auto& def = getShaderDefinition(BuiltinShader::Glow);
        REQUIRE(def.fragmentCode.find("smoothstep") != std::string_view::npos);
    }

    SECTION("given Glow shader when getting definition then fragment contains glow_intensity") {
        const auto& def = getShaderDefinition(BuiltinShader::Glow);
        REQUIRE(def.fragmentCode.find("glow_intensity") != std::string_view::npos);
    }

    SECTION("given Glow shader when getting definition then fragment contains discard for transparency") {
        const auto& def = getShaderDefinition(BuiltinShader::Glow);
        REQUIRE(def.fragmentCode.find("discard") != std::string_view::npos);
    }
}

TEST_CASE("getShaderDefinition error cases") {
    SECTION("given invalid shader index when getting definition then throws out_of_range") {
        REQUIRE_THROWS_AS(getShaderDefinition(static_cast<BuiltinShader>(100)), std::out_of_range);
    }

    SECTION("given negative shader index when getting definition then throws out_of_range") {
        REQUIRE_THROWS_AS(getShaderDefinition(static_cast<BuiltinShader>(255)), std::out_of_range);
    }
}

TEST_CASE("Shader code validity") {
    SECTION("given all shaders when checking vertex code then all contain @vertex") {
        for (int i = 0; i <= static_cast<int>(BuiltinShader::Glow); i++) {
            const auto& def = getShaderDefinition(static_cast<BuiltinShader>(i));
            REQUIRE(def.vertexCode.find("@vertex") != std::string_view::npos);
        }
    }

    SECTION("given all shaders when checking fragment code then all contain @fragment") {
        for (int i = 0; i <= static_cast<int>(BuiltinShader::Glow); i++) {
            const auto& def = getShaderDefinition(static_cast<BuiltinShader>(i));
            REQUIRE(def.fragmentCode.find("@fragment") != std::string_view::npos);
        }
    }

    SECTION("given all shaders when checking vertex code then all contain fn main") {
        for (int i = 0; i <= static_cast<int>(BuiltinShader::Glow); i++) {
            const auto& def = getShaderDefinition(static_cast<BuiltinShader>(i));
            REQUIRE(def.vertexCode.find("fn main") != std::string_view::npos);
        }
    }

    SECTION("given all shaders when checking fragment code then all contain fn main") {
        for (int i = 0; i <= static_cast<int>(BuiltinShader::Glow); i++) {
            const auto& def = getShaderDefinition(static_cast<BuiltinShader>(i));
            REQUIRE(def.fragmentCode.find("fn main") != std::string_view::npos);
        }
    }

    SECTION("given all shaders when checking vertex code then all reference camera uniforms") {
        for (int i = 0; i <= static_cast<int>(BuiltinShader::Glow); i++) {
            const auto& def = getShaderDefinition(static_cast<BuiltinShader>(i));
            REQUIRE(def.vertexCode.find("CameraUniforms") != std::string_view::npos);
        }
    }

    SECTION("given all shaders when checking then all have PositionNormalUV layout") {
        for (int i = 0; i <= static_cast<int>(BuiltinShader::Glow); i++) {
            const auto& def = getShaderDefinition(static_cast<BuiltinShader>(i));
            REQUIRE(def.vertexLayout == VertexLayoutType::PositionNormalUV);
        }
    }
}

TEST_CASE("Shader consistency") {
    SECTION("given same shader when getting definition twice then returns same reference") {
        const auto& def1 = getShaderDefinition(BuiltinShader::PBR);
        const auto& def2 = getShaderDefinition(BuiltinShader::PBR);
        REQUIRE(&def1 == &def2);
    }

    SECTION("given different shaders when getting definitions then returns different references") {
        const auto& def1 = getShaderDefinition(BuiltinShader::Unlit);
        const auto& def2 = getShaderDefinition(BuiltinShader::PBR);
        REQUIRE(&def1 != &def2);
    }
}
