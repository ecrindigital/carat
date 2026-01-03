#include <catch2/catch_all.hpp>
#include <game_engine/graphics/pbr_material.hpp>

using namespace game_engine::graphics;

TEST_CASE("PBRMaterial construction") {
    SECTION("given new PBRMaterial when constructed then has default albedo white") {
        PBRMaterial material;
        const auto& data = material.getData();
        REQUIRE(data.albedo.r == Catch::Approx(1.0f));
        REQUIRE(data.albedo.g == Catch::Approx(1.0f));
        REQUIRE(data.albedo.b == Catch::Approx(1.0f));
        REQUIRE(data.albedo.a == Catch::Approx(1.0f));
    }

    SECTION("given new PBRMaterial when constructed then metallic is 0") {
        PBRMaterial material;
        REQUIRE(material.getData().metallic == Catch::Approx(0.0f));
    }

    SECTION("given new PBRMaterial when constructed then roughness is 0.5") {
        PBRMaterial material;
        REQUIRE(material.getData().roughness == Catch::Approx(0.5f));
    }

    SECTION("given new PBRMaterial when constructed then ao is 1") {
        PBRMaterial material;
        REQUIRE(material.getData().ao == Catch::Approx(1.0f));
    }

    SECTION("given new PBRMaterial when constructed then emissive is black") {
        PBRMaterial material;
        const auto& data = material.getData();
        REQUIRE(data.emissive.r == Catch::Approx(0.0f));
        REQUIRE(data.emissive.g == Catch::Approx(0.0f));
        REQUIRE(data.emissive.b == Catch::Approx(0.0f));
    }

    SECTION("given new PBRMaterial when constructed then emissive intensity is 0") {
        PBRMaterial material;
        REQUIRE(material.getData().emissiveIntensity == Catch::Approx(0.0f));
    }
}

TEST_CASE("PBRMaterial albedo") {
    SECTION("given PBRMaterial when setting albedo vec4 then stored correctly") {
        PBRMaterial material;
        material.setAlbedo(glm::vec4(0.5f, 0.3f, 0.2f, 0.8f));
        const auto& data = material.getData();
        REQUIRE(data.albedo.r == Catch::Approx(0.5f));
        REQUIRE(data.albedo.g == Catch::Approx(0.3f));
        REQUIRE(data.albedo.b == Catch::Approx(0.2f));
        REQUIRE(data.albedo.a == Catch::Approx(0.8f));
    }

    SECTION("given PBRMaterial when setting albedo vec3 then alpha remains 1") {
        PBRMaterial material;
        material.setAlbedo(glm::vec3(0.5f, 0.3f, 0.2f));
        const auto& data = material.getData();
        REQUIRE(data.albedo.r == Catch::Approx(0.5f));
        REQUIRE(data.albedo.g == Catch::Approx(0.3f));
        REQUIRE(data.albedo.b == Catch::Approx(0.2f));
        REQUIRE(data.albedo.a == Catch::Approx(1.0f));
    }

    SECTION("given PBRMaterial when setting albedo then marks dirty") {
        PBRMaterial material;
        material.setAlbedo(glm::vec4(1.0f));
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("PBRMaterial metallic") {
    SECTION("given PBRMaterial when setting metallic then stored correctly") {
        PBRMaterial material;
        material.setMetallic(0.8f);
        REQUIRE(material.getData().metallic == Catch::Approx(0.8f));
    }

    SECTION("given PBRMaterial when setting metallic to 0 then stored correctly") {
        PBRMaterial material;
        material.setMetallic(0.0f);
        REQUIRE(material.getData().metallic == Catch::Approx(0.0f));
    }

    SECTION("given PBRMaterial when setting metallic to 1 then stored correctly") {
        PBRMaterial material;
        material.setMetallic(1.0f);
        REQUIRE(material.getData().metallic == Catch::Approx(1.0f));
    }

    SECTION("given PBRMaterial when setting metallic then marks dirty") {
        PBRMaterial material;
        material.setMetallic(0.5f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("PBRMaterial roughness") {
    SECTION("given PBRMaterial when setting roughness then stored correctly") {
        PBRMaterial material;
        material.setRoughness(0.3f);
        REQUIRE(material.getData().roughness == Catch::Approx(0.3f));
    }

    SECTION("given PBRMaterial when setting roughness to 0.1 then stored correctly") {
        PBRMaterial material;
        material.setRoughness(0.1f);
        REQUIRE(material.getData().roughness == Catch::Approx(0.1f));
    }

    SECTION("given PBRMaterial when setting roughness to 1 then stored correctly") {
        PBRMaterial material;
        material.setRoughness(1.0f);
        REQUIRE(material.getData().roughness == Catch::Approx(1.0f));
    }

    SECTION("given PBRMaterial when setting roughness then marks dirty") {
        PBRMaterial material;
        material.setRoughness(0.5f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("PBRMaterial ao") {
    SECTION("given PBRMaterial when setting ao then stored correctly") {
        PBRMaterial material;
        material.setAO(0.7f);
        REQUIRE(material.getData().ao == Catch::Approx(0.7f));
    }

    SECTION("given PBRMaterial when setting ao to 0 then stored correctly") {
        PBRMaterial material;
        material.setAO(0.0f);
        REQUIRE(material.getData().ao == Catch::Approx(0.0f));
    }

    SECTION("given PBRMaterial when setting ao then marks dirty") {
        PBRMaterial material;
        material.setAO(0.5f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("PBRMaterial emissive") {
    SECTION("given PBRMaterial when setting emissive then color stored correctly") {
        PBRMaterial material;
        material.setEmissive(glm::vec3(1.0f, 0.5f, 0.0f), 2.0f);
        const auto& data = material.getData();
        REQUIRE(data.emissive.r == Catch::Approx(1.0f));
        REQUIRE(data.emissive.g == Catch::Approx(0.5f));
        REQUIRE(data.emissive.b == Catch::Approx(0.0f));
    }

    SECTION("given PBRMaterial when setting emissive then intensity stored correctly") {
        PBRMaterial material;
        material.setEmissive(glm::vec3(1.0f), 3.5f);
        REQUIRE(material.getData().emissiveIntensity == Catch::Approx(3.5f));
    }

    SECTION("given PBRMaterial when setting emissive with default intensity then intensity is 1") {
        PBRMaterial material;
        material.setEmissive(glm::vec3(1.0f));
        REQUIRE(material.getData().emissiveIntensity == Catch::Approx(1.0f));
    }

    SECTION("given PBRMaterial when setting emissive then marks dirty") {
        PBRMaterial material;
        material.setEmissive(glm::vec3(1.0f), 1.0f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("PBRMaterial dirty flag") {
    SECTION("given new PBRMaterial then is dirty") {
        PBRMaterial material;
        REQUIRE(material.isDirty());
    }

    SECTION("given PBRMaterial when marking dirty then isDirty returns true") {
        PBRMaterial material;
        material.markDirty();
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("PBRMaterial GPU resources without device") {
    SECTION("given PBRMaterial when getBindGroup without GPU resources then returns nullptr") {
        PBRMaterial material;
        REQUIRE(material.getBindGroup() == nullptr);
    }
}

TEST_CASE("PBRMaterial move semantics") {
    SECTION("given PBRMaterial when moved then new material has same data") {
        PBRMaterial original;
        original.setMetallic(0.9f);
        original.setRoughness(0.1f);

        PBRMaterial moved = std::move(original);
        REQUIRE(moved.getData().metallic == Catch::Approx(0.9f));
        REQUIRE(moved.getData().roughness == Catch::Approx(0.1f));
    }
}

TEST_CASE("PBRMaterialData alignment") {
    SECTION("given PBRMaterialData struct then is 16-byte aligned") {
        REQUIRE(alignof(PBRMaterialData) == 16);
    }
}
