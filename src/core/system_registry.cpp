#include <game_engine/core/system_registry.hpp>
#include <algorithm>
#include <spdlog/spdlog.h>

namespace game_engine::core {

    SystemRegistry::SystemRegistry(DIContainer& container, Scheduler& scheduler)
        : m_container(container), m_scheduler(scheduler) {}

    SystemRegistry::~SystemRegistry() = default;

    void SystemRegistry::initialize() {
        if (m_initialized) return;

        m_systems.reserve(m_factories.size());

        for (const auto& factory : m_factories) {
            auto system = factory.factory(m_container);
            if (system) {
                m_systemMap[factory.type] = system.get();
                m_systems.push_back(std::move(system));
                spdlog::info("Initialized system: {}", m_systems.back()->name());
            }
        }

        std::sort(m_systems.begin(), m_systems.end(),
            [](const auto& a, const auto& b) {
                return a->priority() < b->priority();
            });

        m_initialized = true;
        spdlog::info("SystemRegistry initialized with {} systems", m_systems.size());
    }

    void SystemRegistry::execute(float deltaTime) {
        if (!m_initialized) {
            initialize();
        }

        m_scheduler.clear();

        for (const auto& system : m_systems) {
            if (system->isParallelSafe()) {
                m_scheduler.addSystem(std::string(system->name()), [&system, deltaTime]() {
                    system->update(deltaTime);
                });
            }
        }

        m_scheduler.execute();

        for (const auto& system : m_systems) {
            if (!system->isParallelSafe()) {
                system->update(deltaTime);
            }
        }
    }

    void SystemRegistry::clear() {
        m_systems.clear();
        m_systemMap.clear();
        m_factories.clear();
        m_initialized = false;
    }

}
