#include <game_engine/infrastructure/window.hpp>
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>

namespace game_engine::infrastructure {

    class Window::Impl {
    public:
        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
        bool shouldClose = false;
        Uint64 startTime = 0;
    };

    Window::Window() : m_pImpl(std::make_unique<Impl>()) {}

    Window::~Window() {
        shutdown();
    }

    Window::Window(Window&&) noexcept = default;
    Window& Window::operator=(Window&&) noexcept = default;

    core::Result Window::initialize(const WindowConfig& config) {
        spdlog::info("Initializing Window with SDL3");

        if (!SDL_Init(SDL_INIT_VIDEO)) {
            spdlog::error("Failed to initialize SDL3: {}", SDL_GetError());
            return core::Result::Error;
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#ifdef __APPLE__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
#endif
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL;
        if (config.resizable) {
            windowFlags |= SDL_WINDOW_RESIZABLE;
        }

        m_pImpl->window = SDL_CreateWindow(
            config.title.c_str(),
            config.width,
            config.height,
            windowFlags
        );

        if (!m_pImpl->window) {
            spdlog::error("Failed to create SDL3 window: {}", SDL_GetError());
            SDL_Quit();
            return core::Result::Error;
        }

        m_pImpl->glContext = SDL_GL_CreateContext(m_pImpl->window);
        if (!m_pImpl->glContext) {
            spdlog::error("Failed to create OpenGL context: {}", SDL_GetError());
            SDL_DestroyWindow(m_pImpl->window);
            SDL_Quit();
            return core::Result::Error;
        }

        SDL_GL_MakeCurrent(m_pImpl->window, m_pImpl->glContext);
        SDL_GL_SetSwapInterval(config.vsync ? 1 : 0);

        m_pImpl->startTime = SDL_GetPerformanceCounter();

        spdlog::info("Window initialized successfully ({}x{})", config.width, config.height);
        return core::Result::Success;
    }

    void Window::shutdown() {
        if (m_pImpl->glContext) {
            SDL_GL_DestroyContext(m_pImpl->glContext);
            m_pImpl->glContext = nullptr;
        }
        if (m_pImpl->window) {
            SDL_DestroyWindow(m_pImpl->window);
            m_pImpl->window = nullptr;
        }
        SDL_Quit();
    }

    void Window::pollEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    m_pImpl->shouldClose = true;
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    m_pImpl->shouldClose = true;
                    break;
                default:
                    break;
            }
        }
    }

    void Window::swapBuffers() {
        SDL_GL_SwapWindow(m_pImpl->window);
    }

    bool Window::shouldClose() const {
        return m_pImpl->shouldClose;
    }

    void Window::setShouldClose(bool value) {
        m_pImpl->shouldClose = value;
    }

    glm::ivec2 Window::getSize() const {
        int w, h;
        SDL_GetWindowSize(m_pImpl->window, &w, &h);
        return {w, h};
    }

    glm::ivec2 Window::getFramebufferSize() const {
        int w, h;
        SDL_GetWindowSizeInPixels(m_pImpl->window, &w, &h);
        return {w, h};
    }

    float Window::getAspectRatio() const {
        auto size = getSize();
        return size.y > 0 ? static_cast<float>(size.x) / static_cast<float>(size.y) : 1.0f;
    }

    double Window::getTime() const {
        Uint64 now = SDL_GetPerformanceCounter();
        Uint64 freq = SDL_GetPerformanceFrequency();
        return static_cast<double>(now - m_pImpl->startTime) / static_cast<double>(freq);
    }

    void* Window::getNativeHandle() const {
        return m_pImpl->window;
    }

    void* Window::getGLContext() const {
        return m_pImpl->glContext;
    }

    void Window::makeContextCurrent() {
        SDL_GL_MakeCurrent(m_pImpl->window, m_pImpl->glContext);
    }

} // namespace game_engine::infrastructure
