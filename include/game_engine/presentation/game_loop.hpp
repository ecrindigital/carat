#pragma once

#include <game_engine/core/types.hpp>
#include <game_engine/core/scheduler.hpp>
#include <game_engine/infrastructure/ecs_manager.hpp>
#include <game_engine/infrastructure/renderer.hpp>
#include <game_engine/infrastructure/input_manager.hpp>
#include <memory>

namespace game_engine::presentation {
    class GameLoop {
    public:
        GameLoop(
            std::shared_ptr<infrastructure::EcsManager> ecs,
            std::shared_ptr<graphics::Renderer> renderer,
            std::shared_ptr<input::InputManager> inputManager
        );

        ~GameLoop() = default;

        GameLoop(const GameLoop &) = delete;

        GameLoop &operator=(const GameLoop &) = delete;

        GameLoop(GameLoop &&) = delete;

        GameLoop &operator=(GameLoop &&) = delete;

        core::Result initialize();

        void run();

    private:
        void update(float deltaTime);

        void render();

        std::shared_ptr<infrastructure::EcsManager> m_ecs;
        std::shared_ptr<graphics::Renderer> m_renderer;
        std::shared_ptr<input::InputManager> m_inputManager;
        core::Scheduler m_scheduler;

        float m_lastFrame = 0.0f;
    };
} // namespace game_engine::presentation
