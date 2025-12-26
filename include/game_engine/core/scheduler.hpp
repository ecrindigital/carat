#pragma once

#include <taskflow/taskflow.hpp>
#include <functional>
#include <vector>
#include <string>

namespace game_engine::core {

    class Scheduler {
    public:
        using SystemTask = std::function<void()>;

        Scheduler() : m_executor(std::thread::hardware_concurrency()) {}

        explicit Scheduler(size_t numThreads) : m_executor(numThreads) {}

        void addSystem(std::string name, SystemTask task) {
            m_systems.emplace_back(std::move(name), std::move(task));
        }

        void execute() {
            tf::Taskflow taskflow;

            for (const auto& [name, task] : m_systems) {
                taskflow.emplace(task).name(name);
            }

            m_executor.run(taskflow).wait();
            taskflow.clear();
        }

        void clear() {
            m_systems.clear();
        }

        [[nodiscard]] size_t numWorkers() const {
            return m_executor.num_workers();
        }

    private:
        tf::Executor m_executor;
        std::vector<std::pair<std::string, SystemTask>> m_systems;
    };

} // namespace game_engine::core
