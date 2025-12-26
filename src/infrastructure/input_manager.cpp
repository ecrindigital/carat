#include "game_engine/infrastructure/input_manager.hpp"
#include "game_engine/infrastructure/window.hpp"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

namespace game_engine::input {

    class InputManager::Impl {
    public:
        infrastructure::Window* window = nullptr;
        QuitCallback quitCallback;
        const bool* keyboardState = nullptr;
    };

    InputManager::InputManager() : m_pImpl(std::make_unique<Impl>()) {}

    InputManager::~InputManager() = default;

    void InputManager::initialize(infrastructure::Window* window) {
        spdlog::info("Initializing InputManager with SDL3");
        m_pImpl->window = window;
        m_pImpl->keyboardState = SDL_GetKeyboardState(nullptr);
    }

    void InputManager::processInput() {
        if (isKeyPressed(SDL_SCANCODE_ESCAPE)) {
            spdlog::info("Escape key pressed, closing window");
            if (m_pImpl->quitCallback) {
                m_pImpl->quitCallback();
            } else if (m_pImpl->window) {
                m_pImpl->window->setShouldClose(true);
            }
        }
    }

    void InputManager::setQuitCallback(QuitCallback callback) {
        m_pImpl->quitCallback = std::move(callback);
    }

    bool InputManager::isKeyPressed(int scancode) const {
        return m_pImpl->keyboardState && m_pImpl->keyboardState[scancode];
    }

    bool InputManager::isKeyDown(int scancode) const {
        return isKeyPressed(scancode);
    }

} // namespace game_engine::input
