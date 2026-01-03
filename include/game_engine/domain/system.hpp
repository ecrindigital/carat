#pragma once

#include <game_engine/core/system.hpp>
#include <game_engine/infrastructure/ecs_manager.hpp>
#include "component.hpp"
#include <memory>

namespace game_engine::domain::systems {

    class MovementSystem : public core::System {
    public:
        explicit MovementSystem(std::shared_ptr<infrastructure::EcsManager> ecs)
            : m_ecs(std::move(ecs)) {}

        void update(float deltaTime) override {
            auto& registry = m_ecs->getRegistry();

            for (const auto view = registry.view<components::Position, components::Velocity>();
                 auto&& entity : view) {
                auto& [position] = view.get<components::Position>(entity);
                const auto& [velocity] = view.get<components::Velocity>(entity);
                position += velocity * deltaTime;
            }
        }

        std::string_view name() const override { return "MovementSystem"; }
        int priority() const override { return 0; }

    private:
        std::shared_ptr<infrastructure::EcsManager> m_ecs;
    };

}

