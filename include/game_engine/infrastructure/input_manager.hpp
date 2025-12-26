#pragma once

#include <memory>
#include <functional>
#include "game_engine/core/types.hpp"

namespace game_engine::infrastructure {
    class Window;
}

namespace game_engine::input {

    class InputManager {
    public:
        using QuitCallback = std::function<void()>;

        InputManager();
        ~InputManager();

        void initialize(infrastructure::Window* window);
        void processInput();

        void setQuitCallback(QuitCallback callback);

        [[nodiscard]] bool isKeyPressed(int scancode) const;
        [[nodiscard]] bool isKeyDown(int scancode) const;

    private:
        class Impl;
        core::UniquePtr<Impl> m_pImpl;
    };

} // namespace game_engine::input
