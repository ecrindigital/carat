#include "game_engine/infrastructure/renderer.hpp"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <array>

namespace game_engine::graphics {

class Renderer::Impl {
public:
    GLuint m_vao{};
    GLuint m_vbo{};
    GLuint m_shaderProgram{};

    void createTriangle() {
        constexpr auto vertices = std::array {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void *>(nullptr));
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void createShaders() {
        const auto vertexShaderSource = R"(
            #version 330 core
            layout (location = 0) in vec3 aPos;
            void main() {
                gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
            }
        )";

        const auto fragmentShaderSource = R"(
            #version 330 core
            out vec4 FragColor;
            void main() {
                FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
            }
        )";

        const auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
        glCompileShader(vertexShader);

        const auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);

        m_shaderProgram = glCreateProgram();
        glAttachShader(m_shaderProgram, vertexShader);
        glAttachShader(m_shaderProgram, fragmentShader);
        glLinkProgram(m_shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
};

Renderer::Renderer() : m_pImpl(std::make_unique<Impl>()) {}

Renderer::~Renderer() = default;

core::Result Renderer::initialize() const {
    spdlog::info("Initializing Renderer");

    if (!gladLoadGL()) {
        spdlog::warn("GLAD already loaded or GL context issue - continuing anyway");
    }

    m_pImpl->createTriangle();
    m_pImpl->createShaders();

    spdlog::info("Renderer initialized successfully");
    return core::Result::Success;
}

void Renderer::render() const {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_pImpl->m_shaderProgram);
    glBindVertexArray(m_pImpl->m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

} // namespace game_engine::graphics
