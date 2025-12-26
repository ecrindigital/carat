#pragma once

#include <game_engine/core/types.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <functional>
#include <memory>

namespace game_engine::infrastructure {

    struct WindowConfig {
        std::string title = "Game Engine";
        int width = 800;
        int height = 600;
        bool vsync = true;
        bool resizable = true;
    };

    class Window {
    public:
        Window();
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) noexcept;
        Window& operator=(Window&&) noexcept;

        [[nodiscard]] core::Result initialize(const WindowConfig& config);
        void shutdown();

        void pollEvents();
        void swapBuffers();

        [[nodiscard]] bool shouldClose() const;
        void setShouldClose(bool value);

        [[nodiscard]] glm::ivec2 getSize() const;
        [[nodiscard]] glm::ivec2 getFramebufferSize() const;
        [[nodiscard]] float getAspectRatio() const;
        [[nodiscard]] double getTime() const;

        [[nodiscard]] void* getNativeHandle() const;
        [[nodiscard]] void* getGLContext() const;

        void makeContextCurrent();

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

} // namespace game_engine::infrastructure
