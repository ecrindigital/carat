#pragma once

#include <game_engine/core/system.hpp>
#include <game_engine/core/di_container.hpp>
#include <game_engine/core/scheduler.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <typeindex>
#include <unordered_map>

namespace game_engine::core {

    class SystemRegistry {
    public:
        SystemRegistry(DIContainer& container, Scheduler& scheduler);
        ~SystemRegistry();

        SystemRegistry(const SystemRegistry&) = delete;
        SystemRegistry& operator=(const SystemRegistry&) = delete;
        SystemRegistry(SystemRegistry&&) = delete;
        SystemRegistry& operator=(SystemRegistry&&) = delete;

        template<typename T>
        void registerSystem() {
            static_assert(std::is_base_of_v<System, T>, "T must derive from System");
            m_factories.push_back({
                typeid(T),
                [](DIContainer& container) -> std::shared_ptr<System> {
                    return container.resolve<T>();
                }
            });
        }

        template<typename T>
        void registerSystem(std::function<std::shared_ptr<T>(DIContainer&)> factory) {
            static_assert(std::is_base_of_v<System, T>, "T must derive from System");
            m_factories.push_back({
                typeid(T),
                [factory](DIContainer& container) -> std::shared_ptr<System> {
                    return factory(container);
                }
            });
        }

        void initialize();
        void execute(float deltaTime);

        template<typename T>
        T* getSystem() {
            auto it = m_systemMap.find(typeid(T));
            if (it != m_systemMap.end()) {
                return static_cast<T*>(it->second);
            }
            return nullptr;
        }

        void clear();

    private:
        struct SystemFactory {
            std::type_index type;
            std::function<std::shared_ptr<System>(DIContainer&)> factory;
        };

        DIContainer& m_container;
        Scheduler& m_scheduler;
        std::vector<SystemFactory> m_factories;
        std::vector<std::shared_ptr<System>> m_systems;
        std::unordered_map<std::type_index, System*> m_systemMap;
        bool m_initialized = false;
    };

}
