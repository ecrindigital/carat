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
            registerType<T>([](DIContainer &container) {
                return std::make_shared<T>();
            });
        }

        template<typename T>
        void registerType(std::function<std::shared_ptr<T>(DIContainer &)> factory) {
            m_factories[typeid(T)] = [factory](DIContainer &container) {
                return factory(container);
            };
        }

        template<typename T, typename Impl>
        void registerInterface() {
            static_assert(std::is_base_of_v<T, Impl>, "Impl must inherit from T");
            registerType<T>([this](DIContainer &container) {
                return std::static_pointer_cast<T>(resolve<Impl>());
            });
        }

        template<typename T>
        std::shared_ptr<T> resolve() {
            auto it = m_factories.find(typeid(T));
            if (it == m_factories.end()) {
                throw std::runtime_error(std::string("Type not registered: ") + typeid(T).name());
            }
            return std::static_pointer_cast<T>(it->second(*this));
        }

    private:
        std::unordered_map<std::type_index, std::function<std::shared_ptr<void>(DIContainer &)> > m_factories;
    };
} // namespace game_engine::core
