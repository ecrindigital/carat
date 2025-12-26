#pragma once

#include <game_engine/core/types.hpp>
#include <memory>

namespace game_engine::infrastructure {
    class Window;
}

namespace game_engine::ui {

    struct DebugStats {
        float fps = 0.0f;
        float frameTime = 0.0f;
        int entityCount = 0;
        size_t numWorkers = 0;
    };

    class DebugUI {
    public:
        DebugUI();
        ~DebugUI();

        DebugUI(const DebugUI&) = delete;
        DebugUI& operator=(const DebugUI&) = delete;

        [[nodiscard]] core::Result initialize(infrastructure::Window* window);
        void shutdown();

        void beginFrame();
        void endFrame();

        void render(const DebugStats& stats);

        [[nodiscard]] bool isEnabled() const { return m_enabled; }
        void setEnabled(bool enabled) { m_enabled = enabled; }
        void toggle() { m_enabled = !m_enabled; }

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
        bool m_enabled = true;
    };

} // namespace game_engine::ui
