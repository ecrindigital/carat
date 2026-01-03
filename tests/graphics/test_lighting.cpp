#include <catch2/catch_all.hpp>
#include <game_engine/graphics/lighting.hpp>

using namespace game_engine::graphics;

TEST_CASE("LightingManager ambient light") {
    SECTION("given new manager when setting ambient then data reflects change") {
        LightingManager manager;
        manager.setAmbient({0.5f, 0.3f, 0.2f}, 0.8f);
        const auto& data = manager.getData();
        REQUIRE(data.ambientColor.r == Catch::Approx(0.5f * 0.8f));
        REQUIRE(data.ambientColor.g == Catch::Approx(0.3f * 0.8f));
        REQUIRE(data.ambientColor.b == Catch::Approx(0.2f * 0.8f));
    }

    SECTION("given manager when setting ambient then marks dirty") {
        LightingManager manager;
        manager.setAmbient({1.0f, 1.0f, 1.0f}, 1.0f);
        REQUIRE(manager.isDirty());
    }
}

TEST_CASE("LightingManager camera position") {
    SECTION("given manager when setting camera position then data reflects change") {
        LightingManager manager;
        manager.setCameraPosition({10.0f, 20.0f, 30.0f});
        const auto& data = manager.getData();
        REQUIRE(data.cameraPosition.x == 10.0f);
        REQUIRE(data.cameraPosition.y == 20.0f);
        REQUIRE(data.cameraPosition.z == 30.0f);
    }

    SECTION("given manager when setting camera position then marks dirty") {
        LightingManager manager;
        manager.setCameraPosition({0.0f, 0.0f, 5.0f});
        REQUIRE(manager.isDirty());
    }
}

TEST_CASE("LightingManager directional lights") {
    SECTION("given new manager when adding directional light then count increases") {
        LightingManager manager;
        manager.addDirectionalLight({0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);
        REQUIRE(manager.getData().numDirectionalLights == 1);
    }

    SECTION("given manager when adding 4 directional lights then all stored") {
        LightingManager manager;
        for (int i = 0; i < 4; i++) {
            manager.addDirectionalLight({0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);
        }
        REQUIRE(manager.getData().numDirectionalLights == 4);
    }

    SECTION("given manager when adding directional light then direction stored correctly") {
        LightingManager manager;
        manager.addDirectionalLight({1.0f, 0.0f, 0.0f}, {1.0f, 0.5f, 0.0f}, 2.0f);
        const auto& data = manager.getData();
        REQUIRE(data.directionalLights[0].direction.x == 1.0f);
        REQUIRE(data.directionalLights[0].color.g == 0.5f);
        REQUIRE(data.directionalLights[0].intensity == 2.0f);
    }

    SECTION("given manager when adding directional light then marks dirty") {
        LightingManager manager;
        manager.addDirectionalLight({0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1.0f);
        REQUIRE(manager.isDirty());
    }
}

TEST_CASE("LightingManager point lights") {
    SECTION("given new manager when adding point light then count increases") {
        LightingManager manager;
        manager.addPointLight({0.0f, 5.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1.0f, 10.0f);
        REQUIRE(manager.getData().numPointLights == 1);
    }

    SECTION("given manager when adding point light then returns valid index") {
        LightingManager manager;
        int index = manager.addPointLight({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        REQUIRE(index >= 0);
    }

    SECTION("given manager when adding 8 point lights then all stored") {
        LightingManager manager;
        for (int i = 0; i < 8; i++) {
            manager.addPointLight({static_cast<float>(i), 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        }
        REQUIRE(manager.getData().numPointLights == 8);
    }

    SECTION("given manager with max lights when adding another then returns -1") {
        LightingManager manager;
        for (int i = 0; i < 8; i++) {
            manager.addPointLight({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        }
        int index = manager.addPointLight({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        REQUIRE(index == -1);
    }

    SECTION("given manager when adding point light then data stored correctly") {
        LightingManager manager;
        manager.addPointLight({1.0f, 2.0f, 3.0f}, {0.5f, 0.6f, 0.7f}, 1.5f, 15.0f);
        const auto& data = manager.getData();
        REQUIRE(data.pointLights[0].position.x == 1.0f);
        REQUIRE(data.pointLights[0].position.y == 2.0f);
        REQUIRE(data.pointLights[0].position.z == 3.0f);
        REQUIRE(data.pointLights[0].color.r == 0.5f);
        REQUIRE(data.pointLights[0].intensity == 1.5f);
        REQUIRE(data.pointLights[0].range == 15.0f);
    }
}

TEST_CASE("LightingManager point light updates") {
    SECTION("given manager with point light when updating position then position changes") {
        LightingManager manager;
        int idx = manager.addPointLight({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        manager.updatePointLight(idx, {5.0f, 10.0f, 15.0f});
        REQUIRE(manager.getData().pointLights[idx].position.x == 5.0f);
        REQUIRE(manager.getData().pointLights[idx].position.y == 10.0f);
        REQUIRE(manager.getData().pointLights[idx].position.z == 15.0f);
    }

    SECTION("given manager with point light when updating all properties then all change") {
        LightingManager manager;
        int idx = manager.addPointLight({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 1.0f, 10.0f);
        manager.updatePointLight(idx, {1.0f, 2.0f, 3.0f}, {0.5f, 0.5f, 0.5f}, 2.0f, 20.0f);
        const auto& light = manager.getData().pointLights[idx];
        REQUIRE(light.position.x == 1.0f);
        REQUIRE(light.color.r == 0.5f);
        REQUIRE(light.intensity == 2.0f);
        REQUIRE(light.range == 20.0f);
    }

    SECTION("given manager when updating invalid index then no crash") {
        LightingManager manager;
        REQUIRE_NOTHROW(manager.updatePointLight(-1, {0.0f, 0.0f, 0.0f}));
        REQUIRE_NOTHROW(manager.updatePointLight(100, {0.0f, 0.0f, 0.0f}));
    }
}

TEST_CASE("LightingManager point light removal") {
    SECTION("given manager with point light when removing then count decreases") {
        LightingManager manager;
        int idx = manager.addPointLight({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        manager.removePointLight(idx);
        REQUIRE(manager.getData().numPointLights == 0);
    }

    SECTION("given manager when removing invalid index then no crash") {
        LightingManager manager;
        REQUIRE_NOTHROW(manager.removePointLight(-1));
        REQUIRE_NOTHROW(manager.removePointLight(100));
    }
}

TEST_CASE("LightingManager clear") {
    SECTION("given manager with lights when clearing then all lights removed") {
        LightingManager manager;
        manager.addDirectionalLight({0.0f, -1.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        manager.addPointLight({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
        manager.clearLights();
        REQUIRE(manager.getData().numDirectionalLights == 0);
        REQUIRE(manager.getData().numPointLights == 0);
    }

    SECTION("given empty manager when clearing then no crash") {
        LightingManager manager;
        REQUIRE_NOTHROW(manager.clearLights());
    }
}

TEST_CASE("LightingManager dirty flag") {
    SECTION("given new manager then is dirty initially") {
        LightingManager manager;
        REQUIRE(manager.isDirty());
    }

    SECTION("given manager when marking dirty then isDirty returns true") {
        LightingManager manager;
        manager.markDirty();
        REQUIRE(manager.isDirty());
    }
}

TEST_CASE("LightingData alignment") {
    SECTION("given LightingData struct then is 16-byte aligned") {
        REQUIRE(alignof(LightingData) == 16);
    }

    SECTION("given DirectionalLight struct then has correct size for GPU") {
        REQUIRE(sizeof(DirectionalLight) == 32);
    }

    SECTION("given PointLight struct then has correct size for GPU") {
        REQUIRE(sizeof(PointLight) == 32);
    }
}
