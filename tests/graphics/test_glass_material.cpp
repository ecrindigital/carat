#include <catch2/catch_all.hpp>
#include <game_engine/graphics/glass_material.hpp>

using namespace game_engine::graphics;

TEST_CASE("GlassMaterial construction") {
    SECTION("given new GlassMaterial when constructed then has default tint") {
        GlassMaterial material;
        const auto& data = material.getData();
        REQUIRE(data.tint.r == Catch::Approx(1.0f));
        REQUIRE(data.tint.g == Catch::Approx(1.0f));
        REQUIRE(data.tint.b == Catch::Approx(1.0f));
        REQUIRE(data.tint.a == Catch::Approx(0.1f));
    }

    SECTION("given new GlassMaterial when constructed then ior is 1.5") {
        GlassMaterial material;
        REQUIRE(material.getData().ior == Catch::Approx(1.5f));
    }

    SECTION("given new GlassMaterial when constructed then roughness is 0") {
        GlassMaterial material;
        REQUIRE(material.getData().roughness == Catch::Approx(0.0f));
    }

    SECTION("given new GlassMaterial when constructed then fresnel power is 5") {
        GlassMaterial material;
        REQUIRE(material.getData().fresnelPower == Catch::Approx(5.0f));
    }

    SECTION("given new GlassMaterial when constructed then reflectivity is 0.04") {
        GlassMaterial material;
        REQUIRE(material.getData().reflectivity == Catch::Approx(0.04f));
    }

    SECTION("given new GlassMaterial when constructed then thickness is 0.1") {
        GlassMaterial material;
        REQUIRE(material.getData().thickness == Catch::Approx(0.1f));
    }

    SECTION("given new GlassMaterial when constructed then dispersion is 0") {
        GlassMaterial material;
        REQUIRE(material.getData().dispersion == Catch::Approx(0.0f));
    }
}

TEST_CASE("GlassMaterial tint") {
    SECTION("given GlassMaterial when setting tint vec4 then stored correctly") {
        GlassMaterial material;
        material.setTint(glm::vec4(0.5f, 0.8f, 1.0f, 0.3f));
        const auto& data = material.getData();
        REQUIRE(data.tint.r == Catch::Approx(0.5f));
        REQUIRE(data.tint.g == Catch::Approx(0.8f));
        REQUIRE(data.tint.b == Catch::Approx(1.0f));
        REQUIRE(data.tint.a == Catch::Approx(0.3f));
    }

    SECTION("given GlassMaterial when setting tint vec3 then opacity is set") {
        GlassMaterial material;
        material.setTint(glm::vec3(0.5f, 0.8f, 1.0f), 0.2f);
        const auto& data = material.getData();
        REQUIRE(data.tint.r == Catch::Approx(0.5f));
        REQUIRE(data.tint.g == Catch::Approx(0.8f));
        REQUIRE(data.tint.b == Catch::Approx(1.0f));
        REQUIRE(data.tint.a == Catch::Approx(0.2f));
    }

    SECTION("given GlassMaterial when setting tint vec3 with default then opacity is 0.1") {
        GlassMaterial material;
        material.setTint(glm::vec3(1.0f, 0.0f, 0.0f));
        REQUIRE(material.getData().tint.a == Catch::Approx(0.1f));
    }

    SECTION("given GlassMaterial when setting tint then marks dirty") {
        GlassMaterial material;
        material.setTint(glm::vec4(1.0f));
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("GlassMaterial IOR") {
    SECTION("given GlassMaterial when setting ior then stored correctly") {
        GlassMaterial material;
        material.setIOR(1.33f);
        REQUIRE(material.getData().ior == Catch::Approx(1.33f));
    }

    SECTION("given GlassMaterial when setting ior to glass value then stored correctly") {
        GlassMaterial material;
        material.setIOR(1.52f);
        REQUIRE(material.getData().ior == Catch::Approx(1.52f));
    }

    SECTION("given GlassMaterial when setting ior to diamond value then stored correctly") {
        GlassMaterial material;
        material.setIOR(2.42f);
        REQUIRE(material.getData().ior == Catch::Approx(2.42f));
    }

    SECTION("given GlassMaterial when setting ior then marks dirty") {
        GlassMaterial material;
        material.setIOR(1.5f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("GlassMaterial roughness") {
    SECTION("given GlassMaterial when setting roughness then stored correctly") {
        GlassMaterial material;
        material.setRoughness(0.3f);
        REQUIRE(material.getData().roughness == Catch::Approx(0.3f));
    }

    SECTION("given GlassMaterial when setting roughness to 0 then stored correctly") {
        GlassMaterial material;
        material.setRoughness(0.0f);
        REQUIRE(material.getData().roughness == Catch::Approx(0.0f));
    }

    SECTION("given GlassMaterial when setting roughness to 1 then stored correctly") {
        GlassMaterial material;
        material.setRoughness(1.0f);
        REQUIRE(material.getData().roughness == Catch::Approx(1.0f));
    }

    SECTION("given GlassMaterial when setting roughness then marks dirty") {
        GlassMaterial material;
        material.setRoughness(0.5f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("GlassMaterial fresnel power") {
    SECTION("given GlassMaterial when setting fresnel power then stored correctly") {
        GlassMaterial material;
        material.setFresnelPower(3.0f);
        REQUIRE(material.getData().fresnelPower == Catch::Approx(3.0f));
    }

    SECTION("given GlassMaterial when setting fresnel power high then stored correctly") {
        GlassMaterial material;
        material.setFresnelPower(10.0f);
        REQUIRE(material.getData().fresnelPower == Catch::Approx(10.0f));
    }

    SECTION("given GlassMaterial when setting fresnel power then marks dirty") {
        GlassMaterial material;
        material.setFresnelPower(2.0f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("GlassMaterial reflectivity") {
    SECTION("given GlassMaterial when setting reflectivity then stored correctly") {
        GlassMaterial material;
        material.setReflectivity(0.1f);
        REQUIRE(material.getData().reflectivity == Catch::Approx(0.1f));
    }

    SECTION("given GlassMaterial when setting reflectivity to 0 then stored correctly") {
        GlassMaterial material;
        material.setReflectivity(0.0f);
        REQUIRE(material.getData().reflectivity == Catch::Approx(0.0f));
    }

    SECTION("given GlassMaterial when setting reflectivity then marks dirty") {
        GlassMaterial material;
        material.setReflectivity(0.5f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("GlassMaterial thickness") {
    SECTION("given GlassMaterial when setting thickness then stored correctly") {
        GlassMaterial material;
        material.setThickness(0.5f);
        REQUIRE(material.getData().thickness == Catch::Approx(0.5f));
    }

    SECTION("given GlassMaterial when setting thickness to 0 then stored correctly") {
        GlassMaterial material;
        material.setThickness(0.0f);
        REQUIRE(material.getData().thickness == Catch::Approx(0.0f));
    }

    SECTION("given GlassMaterial when setting thickness then marks dirty") {
        GlassMaterial material;
        material.setThickness(0.2f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("GlassMaterial dispersion") {
    SECTION("given GlassMaterial when setting dispersion then stored correctly") {
        GlassMaterial material;
        material.setDispersion(0.05f);
        REQUIRE(material.getData().dispersion == Catch::Approx(0.05f));
    }

    SECTION("given GlassMaterial when setting dispersion to 0 then stored correctly") {
        GlassMaterial material;
        material.setDispersion(0.0f);
        REQUIRE(material.getData().dispersion == Catch::Approx(0.0f));
    }

    SECTION("given GlassMaterial when setting dispersion then marks dirty") {
        GlassMaterial material;
        material.setDispersion(0.1f);
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("GlassMaterial dirty flag") {
    SECTION("given new GlassMaterial then is dirty") {
        GlassMaterial material;
        REQUIRE(material.isDirty());
    }

    SECTION("given GlassMaterial when marking dirty then isDirty returns true") {
        GlassMaterial material;
        material.markDirty();
        REQUIRE(material.isDirty());
    }
}

TEST_CASE("GlassMaterial GPU resources without device") {
    SECTION("given GlassMaterial when getBindGroup without GPU resources then returns nullptr") {
        GlassMaterial material;
        REQUIRE(material.getBindGroup() == nullptr);
    }
}

TEST_CASE("GlassMaterial move semantics") {
    SECTION("given GlassMaterial when moved then new material has same data") {
        GlassMaterial original;
        original.setIOR(1.8f);
        original.setRoughness(0.2f);

        GlassMaterial moved = std::move(original);
        REQUIRE(moved.getData().ior == Catch::Approx(1.8f));
        REQUIRE(moved.getData().roughness == Catch::Approx(0.2f));
    }
}

TEST_CASE("GlassMaterialData alignment") {
    SECTION("given GlassMaterialData struct then is 16-byte aligned") {
        REQUIRE(alignof(GlassMaterialData) == 16);
    }
}

TEST_CASE("GlassMaterial realistic scenarios") {
    SECTION("given GlassMaterial when configuring as water then values correct") {
        GlassMaterial material;
        material.setTint(glm::vec3(0.8f, 0.9f, 1.0f), 0.1f);
        material.setIOR(1.33f);
        material.setRoughness(0.0f);
        const auto& data = material.getData();
        REQUIRE(data.ior == Catch::Approx(1.33f));
        REQUIRE(data.roughness == Catch::Approx(0.0f));
    }

    SECTION("given GlassMaterial when configuring as diamond then values correct") {
        GlassMaterial material;
        material.setTint(glm::vec3(1.0f, 1.0f, 1.0f), 0.05f);
        material.setIOR(2.42f);
        material.setDispersion(0.044f);
        const auto& data = material.getData();
        REQUIRE(data.ior == Catch::Approx(2.42f));
        REQUIRE(data.dispersion == Catch::Approx(0.044f));
    }

    SECTION("given GlassMaterial when configuring as frosted glass then values correct") {
        GlassMaterial material;
        material.setRoughness(0.5f);
        material.setIOR(1.52f);
        const auto& data = material.getData();
        REQUIRE(data.roughness == Catch::Approx(0.5f));
        REQUIRE(data.ior == Catch::Approx(1.52f));
    }
}
