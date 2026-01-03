#include <game_engine/presentation/game_loop.hpp>
#include <game_engine/core/profiling.hpp>
#include <game_engine/domain/system.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

#ifndef USE_WEBGPU
#include <glad/glad.h>
#include <SDL3/SDL.h>
#endif

namespace game_engine::presentation {
    GameLoop::GameLoop(
        std::shared_ptr<infrastructure::EcsManager> ecs,
        std::shared_ptr<input::InputManager> inputManager
    ) : m_ecs(std::move(ecs))
        , m_inputManager(std::move(inputManager))
        , m_window(std::make_unique<infrastructure::Window>()) {
        if (!m_ecs || !m_inputManager) {
            throw std::invalid_argument("GameLoop dependencies cannot be null");
        }
        spdlog::info("GameLoop constructed");
    }

    GameLoop::~GameLoop() = default;

    core::Result GameLoop::initialize() {
        spdlog::info("Initializing GameLoop");

        infrastructure::WindowConfig config;
        config.title = "Axolotl Engine";
        config.width = 800;
        config.height = 600;
        config.vsync = true;

        if (m_window->initialize(config) != core::Result::Success) {
            spdlog::error("Failed to initialize Window");
            return core::Result::Error;
        }

#ifdef USE_WEBGPU
        spdlog::info("Using WebGPU backend");
        m_renderer = std::make_unique<graphics::WGPURenderer>();
        if (m_renderer->initialize(m_window.get()) != core::Result::Success) {
            spdlog::error("Failed to initialize WebGPU Renderer");
            return core::Result::Error;
        }
#else
        spdlog::info("Using OpenGL backend");
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(SDL_GL_GetProcAddress))) {
            spdlog::error("Failed to initialize GLAD");
            return core::Result::Error;
        }

        m_renderer = std::make_shared<graphics::Renderer>();
        if (m_renderer->initialize() != core::Result::Success) {
            spdlog::error("Failed to initialize Renderer");
            return core::Result::Error;
        }
#endif

        m_inputManager->initialize(m_window.get());

        spdlog::info("GameLoop initialized successfully");
        return core::Result::Success;
    }

    void GameLoop::run() {
        spdlog::info("Starting main game loop");

        while (!m_window->shouldClose()) {
            auto currentFrame = static_cast<float>(m_window->getTime());
            float deltaTime = currentFrame - m_lastFrame;
            m_lastFrame = currentFrame;

            m_window->pollEvents();
            m_inputManager->processInput();
            update(deltaTime);
            render();

#ifndef USE_WEBGPU
            m_window->swapBuffers();
#endif

            AXOLOTL_FRAME;
        }

        spdlog::info("Exiting main game loop");
    }

    void GameLoop::update(float deltaTime) {
        AXOLOTL_ZONE_NAMED("Update");
        (void)deltaTime;
    }

    void GameLoop::render() {
        AXOLOTL_ZONE_NAMED("Render");

#ifdef USE_WEBGPU
        m_renderer->render();
#else
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_renderer->render();
#endif
    }
} // namespace game_engine::presentation
