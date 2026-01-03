#include <game_engine/core/di_container.hpp>
#include <game_engine/presentation/game_loop.hpp>
#include <game_engine/infrastructure/ecs_manager.hpp>
#include <game_engine/infrastructure/input_manager.hpp>
#include <spdlog/spdlog.h>

#include <game_engine/graphics/mesh.hpp>
#include <game_engine/graphics/material.hpp>
#include <game_engine/graphics/wgpu_renderer.hpp>
#include <game_engine/graphics/shader_registry.hpp>
#include <array>

int main() {
    try {
        game_engine::core::DIContainer container;

        container.registerType<game_engine::infrastructure::EcsManager>();
        container.registerType<game_engine::input::InputManager>();

        container.registerType<game_engine::presentation::GameLoop>(
            [](game_engine::core::DIContainer &c) {
                return std::make_shared<game_engine::presentation::GameLoop>(
                    c.resolve<game_engine::infrastructure::EcsManager>(),
                    c.resolve<game_engine::input::InputManager>()
                );
            }
        );

        auto gameLoop = container.resolve<game_engine::presentation::GameLoop>();
        if (gameLoop->initialize() != game_engine::core::Result::Success) {
            spdlog::error("Failed to initialize GameLoop");
            return 1;
        }

        using namespace game_engine::graphics;

        auto material = Material::createUnlit({1.0f, 0.5f, 0.2f, 1.0f});

        std::array<VertexPosition, 3> vertices = {{
            {-0.5f, -0.5f, 0.0f},
            { 0.5f, -0.5f, 0.0f},
            { 0.0f,  0.5f, 0.0f}
        }};

        auto mesh = std::make_unique<Mesh>();
        mesh->setVertices(std::span{vertices});
        mesh->setMaterial(material.get());

        gameLoop->run();

        return 0;
    } catch (const std::exception &e) {
        spdlog::error("An error occurred: {}", e.what());
        return 1;
    }
}
