#include <catch2/catch_all.hpp>
#include <game_engine/graphics/material.hpp>
#include <game_engine/graphics/builtin_shaders.hpp>

using namespace game_engine::graphics;

TEST_CASE("Material factory methods") {
    SECTION("given createUnlit when called then returns unlit shader type") {
        auto material = Material::createUnlit({1.0f, 0.0f, 0.0f, 1.0f});
        REQUIRE(material->getShader() == BuiltinShader::Unlit);
    }

    SECTION("given createUnlit when called then stores color") {
        auto material = Material::createUnlit({0.5f, 0.3f, 0.2f, 1.0f});
        auto color = material->getColor();
        REQUIRE(color.r == Catch::Approx(0.5f));
        REQUIRE(color.g == Catch::Approx(0.3f));
        REQUIRE(color.b == Catch::Approx(0.2f));
        REQUIRE(color.a == Catch::Approx(1.0f));
    }

    SECTION("given createGlow when called then returns glow shader type") {
        auto material = Material::createGlow({1.0f, 1.0f, 0.0f, 1.0f});
        REQUIRE(material->getShader() == BuiltinShader::Glow);
    }

    SECTION("given createSprite when called then returns sprite shader type") {
        auto material = Material::createSprite(nullptr);
        REQUIRE(material->getShader() == BuiltinShader::Sprite);
    }

    SECTION("given createUnlitTextured when called then returns unlit textured shader") {
        auto material = Material::createUnlitTextured(nullptr);
        REQUIRE(material->getShader() == BuiltinShader::UnlitTextured);
    }
}

TEST_CASE("Material color operations") {
    SECTION("given material when setting color then color changes") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setColor({0.1f, 0.2f, 0.3f, 0.4f});
        auto color = material->getColor();
        REQUIRE(color.r == Catch::Approx(0.1f));
        REQUIRE(color.g == Catch::Approx(0.2f));
        REQUIRE(color.b == Catch::Approx(0.3f));
        REQUIRE(color.a == Catch::Approx(0.4f));
    }

    SECTION("given material when setting color then marks dirty") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setColor({0.5f, 0.5f, 0.5f, 1.0f});
        REQUIRE(material->isDirty());
    }
}

TEST_CASE("Material UV operations") {
    SECTION("given material when setting UV offset then offset stored") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setUVOffset({0.5f, 0.25f});
        auto offset = material->getUVOffset();
        REQUIRE(offset.x == Catch::Approx(0.5f));
        REQUIRE(offset.y == Catch::Approx(0.25f));
    }

    SECTION("given material when setting UV scale then scale stored") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setUVScale({2.0f, 3.0f});
        auto scale = material->getUVScale();
        REQUIRE(scale.x == Catch::Approx(2.0f));
        REQUIRE(scale.y == Catch::Approx(3.0f));
    }

    SECTION("given material when setting UV offset then marks dirty") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setUVOffset({0.0f, 0.0f});
        REQUIRE(material->isDirty());
    }

    SECTION("given material when setting UV scale then marks dirty") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setUVScale({1.0f, 1.0f});
        REQUIRE(material->isDirty());
    }
}

TEST_CASE("Material flip operations") {
    SECTION("given material when setting flip X then flip stored") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setFlip(true, false);
        REQUIRE(material->getFlipX() == true);
        REQUIRE(material->getFlipY() == false);
    }

    SECTION("given material when setting flip Y then flip stored") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setFlip(false, true);
        REQUIRE(material->getFlipX() == false);
        REQUIRE(material->getFlipY() == true);
    }

    SECTION("given material when setting both flips then both stored") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setFlip(true, true);
        REQUIRE(material->getFlipX() == true);
        REQUIRE(material->getFlipY() == true);
    }

    SECTION("given material when setting flip then marks dirty") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->setFlip(true, true);
        REQUIRE(material->isDirty());
    }
}

TEST_CASE("Material texture") {
    SECTION("given material when created without texture then getTexture returns nullptr") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        REQUIRE(material->getTexture() == nullptr);
    }

    SECTION("given sprite material when created with nullptr then getTexture returns nullptr") {
        auto material = Material::createSprite(nullptr);
        REQUIRE(material->getTexture() == nullptr);
    }
}

TEST_CASE("Material dirty flag") {
    SECTION("given new material then is dirty") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        REQUIRE(material->isDirty());
    }

    SECTION("given material when marking dirty then isDirty returns true") {
        auto material = Material::createUnlit({1.0f, 1.0f, 1.0f, 1.0f});
        material->markDirty();
        REQUIRE(material->isDirty());
    }
}

TEST_CASE("Material data structure") {
    SECTION("given material when getting data then returns valid data") {
        auto material = Material::createUnlit({0.1f, 0.2f, 0.3f, 0.4f});
        material->setUVOffset({0.5f, 0.6f});
        material->setUVScale({2.0f, 3.0f});
        material->setFlip(true, false);

        const auto& data = material->getData();
        REQUIRE(data.color.r == Catch::Approx(0.1f));
        REQUIRE(data.color.g == Catch::Approx(0.2f));
        REQUIRE(data.color.b == Catch::Approx(0.3f));
        REQUIRE(data.color.a == Catch::Approx(0.4f));
        REQUIRE(data.uvOffset.x == Catch::Approx(0.5f));
        REQUIRE(data.uvOffset.y == Catch::Approx(0.6f));
        REQUIRE(data.uvScale.x == Catch::Approx(2.0f));
        REQUIRE(data.uvScale.y == Catch::Approx(3.0f));
        REQUIRE(data.flipX == 1);
        REQUIRE(data.flipY == 0);
    }
}

TEST_CASE("MaterialData alignment") {
    SECTION("given MaterialData struct then is 16-byte aligned") {
        REQUIRE(alignof(MaterialData) == 16);
    }
}

TEST_CASE("Material move semantics") {
    SECTION("given material when moved then new material has same data") {
        auto original = Material::createUnlit({0.5f, 0.5f, 0.5f, 1.0f});
        original->setUVScale({2.0f, 2.0f});

        auto moved = std::move(*original);
        REQUIRE(moved.getColor().r == Catch::Approx(0.5f));
        REQUIRE(moved.getUVScale().x == Catch::Approx(2.0f));
    }
}
