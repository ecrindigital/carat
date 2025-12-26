#include <game_engine/core/di_container.hpp>
#include <game_engine/presentation/game_loop.hpp>
#include <game_engine/infrastructure/ecs_manager.hpp>
#include <game_engine/infrastructure/input_manager.hpp>
#include <spdlog/spdlog.h>

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
        if (gameLoop->initialize() == game_engine::core::Result::Success) {
            gameLoop->run();
        } else {
            spdlog::error("Failed to initialize GameLoop");
        }

        return 0;
    } catch (const std::exception &e) {
        spdlog::error("An error occurred: {}", e.what());
        return 1;
    }
}
