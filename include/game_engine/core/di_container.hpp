#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

namespace game_engine::core {
    class DIContainer {
    public:
        template<typename T>
        void registerType() {
            registerType<T>([](DIContainer&) {
                return std::make_shared<T>();
            });
        }

        template<typename T>
        void registerType(std::function<std::shared_ptr<T>(DIContainer &)> factory) {
            m_factories[typeid(T)] = [factory](DIContainer &container) {
                return factory(container);
            };
        }

        template<typename T>
        void registerSingleton() {
            registerSingleton<T>([](DIContainer&) {
                return std::make_shared<T>();
            });
        }

        template<typename T>
        void registerSingleton(std::function<std::shared_ptr<T>(DIContainer &)> factory) {
            m_singletonFactories[typeid(T)] = [factory](DIContainer &container) {
                return factory(container);
            };
        }

        template<typename T, typename Impl>
        void registerInterface() {
            static_assert(std::is_base_of_v<T, Impl>, "Impl must inherit from T");
            registerType<T>([this](DIContainer&) {
                return std::static_pointer_cast<T>(resolve<Impl>());
            });
        }

        template<typename T>
        std::shared_ptr<T> resolve() {
            auto singletonIt = m_singletons.find(typeid(T));
            if (singletonIt != m_singletons.end()) {
                return std::static_pointer_cast<T>(singletonIt->second);
            }

            auto singletonFactoryIt = m_singletonFactories.find(typeid(T));
            if (singletonFactoryIt != m_singletonFactories.end()) {
                auto instance = singletonFactoryIt->second(*this);
                m_singletons[typeid(T)] = instance;
                return std::static_pointer_cast<T>(instance);
            }

            auto it = m_factories.find(typeid(T));
            if (it == m_factories.end()) {
                throw std::runtime_error(std::string("Type not registered: ") + typeid(T).name());
            }
            return std::static_pointer_cast<T>(it->second(*this));
        }

        template<typename T>
        bool isRegistered() const {
            return m_factories.contains(typeid(T)) ||
                   m_singletonFactories.contains(typeid(T)) ||
                   m_singletons.contains(typeid(T));
        }

    private:
        std::unordered_map<std::type_index, std::function<std::shared_ptr<void>(DIContainer &)>> m_factories;
        std::unordered_map<std::type_index, std::function<std::shared_ptr<void>(DIContainer &)>> m_singletonFactories;
        std::unordered_map<std::type_index, std::shared_ptr<void>> m_singletons;
    };
}

