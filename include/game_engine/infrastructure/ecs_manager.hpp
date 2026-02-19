#pragma once
#include <flecs.h>

namespace game_engine::infrastructure {
    class EcsManager {
    public:
        EcsManager() = default;
        ~EcsManager() = default;

        flecs::entity createEntity() {
            return m_world.entity();
        }

        void destroyEntity(flecs::entity entity) {
            entity.destruct();
        }

        template<typename T, typename... Args>
        T& addComponent(flecs::entity entity, Args&&... args) {
            entity.set(T{std::forward<Args>(args)...});
            return *entity.get_mut<T>();
        }

        template<typename T>
        void removeComponent(flecs::entity entity) {
            entity.remove<T>();
        }

        template<typename T>
        const T& getComponent(flecs::entity entity) {
            return *entity.get<T>();
        }

        flecs::world& world() {
            return m_world;
        }

    private:
        flecs::world m_world;
    };
} // namespace game_engine::infrastructure
