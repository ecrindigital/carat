#include <game_engine/game.hpp>
#include <game_engine/graphics/mesh.hpp>
#include <game_engine/graphics/material.hpp>
#include <spdlog/spdlog.h>
#include <cmath>

int main() {
    try {
        game_engine::Game game("Glass Sphere Demo", 1280, 720);

        auto* sphere = game.createSphere(1.0f, 64);
        auto* sphereMaterial = game.createUnlitMaterial({0.3f, 0.6f, 0.9f, 1.0f});
        sphere->setMaterial(sphereMaterial);

        auto* floor = game.createPlane(10.0f, 10.0f);
        auto* floorMaterial = game.createUnlitMaterial({0.2f, 0.2f, 0.25f, 1.0f});
        floor->setMaterial(floorMaterial);
        floor->setPosition({0.0f, -1.5f, 0.0f});
        floor->setRotation({-1.57f, 0.0f, 0.0f});

        auto* cube = game.createCube(0.5f);
        auto* cubeMaterial = game.createUnlitMaterial({0.9f, 0.3f, 0.2f, 1.0f});
        cube->setMaterial(cubeMaterial);
        cube->setPosition({2.0f, -1.0f, 0.0f});

        game.addMesh(sphere);
        game.addMesh(floor);
        game.addMesh(cube);

        game.setCameraPosition({0.0f, 1.0f, 4.0f});
        game.setCameraLookAt({0.0f, 0.0f, 0.0f});
        game.setPerspective(45.0f, 0.1f, 100.0f);

        game.setAmbientLight({0.1f, 0.1f, 0.15f}, 0.3f);
        game.addDirectionalLight({-0.5f, -1.0f, -0.3f}, {1.0f, 0.95f, 0.9f}, 1.0f);

        float rotation = 0.0f;
        float cubeRotation = 0.0f;
        game.onUpdate([&](float dt) {
            rotation += dt * 0.5f;
            cubeRotation += dt * 1.0f;
            sphere->setRotation({0.0f, rotation, 0.0f});
            cube->setRotation({cubeRotation, cubeRotation * 0.7f, 0.0f});
        });

        game.run();
        return 0;

    } catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }
}
