#include <game_engine/ui/debug_ui.hpp>
#include <game_engine/infrastructure/window.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>
#include <glad/glad.h>

namespace game_engine::ui {

    class DebugUI::Impl {
    public:
        infrastructure::Window* window = nullptr;
        bool initialized = false;
    };

    DebugUI::DebugUI() : m_pImpl(std::make_unique<Impl>()) {}

    DebugUI::~DebugUI() {
        shutdown();
    }

    core::Result DebugUI::initialize(infrastructure::Window* window) {
        spdlog::info("Initializing DebugUI with ImGui");
        m_pImpl->window = window;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        m_pImpl->initialized = true;
        spdlog::info("DebugUI initialized successfully");
        return core::Result::Success;
    }

    void DebugUI::shutdown() {
        if (m_pImpl->initialized) {
            ImGui::DestroyContext();
            m_pImpl->initialized = false;
        }
    }

    void DebugUI::beginFrame() {
        if (!m_pImpl->initialized || !m_enabled) return;

        ImGuiIO& io = ImGui::GetIO();
        auto size = m_pImpl->window->getSize();
        io.DisplaySize = ImVec2(static_cast<float>(size.x), static_cast<float>(size.y));

        static Uint64 lastTime = SDL_GetPerformanceCounter();
        Uint64 currentTime = SDL_GetPerformanceCounter();
        Uint64 freq = SDL_GetPerformanceFrequency();
        io.DeltaTime = static_cast<float>(currentTime - lastTime) / static_cast<float>(freq);
        if (io.DeltaTime <= 0.0f) io.DeltaTime = 1.0f / 60.0f;
        lastTime = currentTime;

        ImGui::NewFrame();
    }

    void DebugUI::endFrame() {
        if (!m_pImpl->initialized || !m_enabled) return;

        ImGui::Render();
    }

    void DebugUI::render(const DebugStats& stats) {
        if (!m_pImpl->initialized || !m_enabled) return;

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(250, 150), ImGuiCond_FirstUseEver);

        if (ImGui::Begin("Debug Stats")) {
            ImGui::Text("FPS: %.1f", stats.fps);
            ImGui::Text("Frame Time: %.3f ms", stats.frameTime * 1000.0f);
            ImGui::Separator();
            ImGui::Text("Entities: %d", stats.entityCount);
            ImGui::Text("Worker Threads: %zu", stats.numWorkers);
            ImGui::Separator();
            if (ImGui::Button("Toggle Profiling")) {
                spdlog::info("Profiling toggled");
            }
        }
        ImGui::End();
    }

} // namespace game_engine::ui
