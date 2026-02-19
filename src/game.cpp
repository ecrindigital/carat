#include <game_engine/game.hpp>
#include <game_engine/infrastructure/window.hpp>
#include <game_engine/graphics/wgpu_renderer.hpp>
#include <game_engine/graphics/shader_registry.hpp>
#include <game_engine/graphics/mesh.hpp>
#include <game_engine/graphics/material.hpp>
#include <game_engine/graphics/pbr_material.hpp>
#include <game_engine/graphics/glass_material.hpp>
#include <game_engine/graphics/lighting.hpp>
#include <game_engine/graphics/primitives.hpp>
#include <game_engine/graphics/texture_loader.hpp>
#include <game_engine/audio/audio_manager.hpp>
#include <flecs.h>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>
#include <vector>

namespace game_engine {

    class Game::Impl {
    public:
        GameConfig config;

        flecs::world world;

        std::unique_ptr<infrastructure::Window> window;
        std::unique_ptr<graphics::WGPURenderer> renderer;

        std::vector<std::unique_ptr<graphics::Mesh>> ownedMeshes;
        std::vector<std::unique_ptr<graphics::Material>> ownedMaterials;
        std::vector<std::unique_ptr<graphics::PBRMaterial>> ownedPBRMaterials;
        std::vector<std::unique_ptr<graphics::GlassMaterial>> ownedGlassMaterials;
        std::vector<std::unique_ptr<graphics::GPUTexture>> ownedTextures;
        std::unique_ptr<graphics::LightingManager> lighting;
        std::unique_ptr<audio::AudioManager> audioManager;

        glm::vec3 cameraPosition{0.0f, 0.0f, 5.0f};
        glm::vec3 cameraTarget{0.0f, 0.0f, 0.0f};
        glm::vec3 cameraUp{0.0f, 1.0f, 0.0f};

        float fov = 45.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;
        bool usePerspective = true;
        float orthoSize = 5.0f;

        bool cameraDirty = true;

        UpdateCallback updateCallback;
        float lastFrameTime = 0.0f;
        bool initialized = false;

        void updateCamera() {
            if (!cameraDirty || !renderer) return;

            glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
            renderer->setViewMatrix(view);

            auto size = window->getSize();
            float aspect = static_cast<float>(size.x) / static_cast<float>(size.y);

            glm::mat4 projection;
            if (usePerspective) {
                projection = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
            } else {
                float hw = orthoSize * aspect * 0.5f;
                float hh = orthoSize * 0.5f;
                projection = glm::ortho(-hw, hw, -hh, hh, nearPlane, farPlane);
            }
            renderer->setProjectionMatrix(projection);

            if (lighting) {
                lighting->setCameraPosition(cameraPosition);
            }

            cameraDirty = false;
        }
    };

    Game::Game() : m_pImpl(std::make_unique<Impl>()) {}

    Game::Game(const std::string& title, int width, int height) : Game() {
        m_pImpl->config.title = title;
        m_pImpl->config.width = width;
        m_pImpl->config.height = height;
    }

    Game::Game(const GameConfig& config) : Game() {
        m_pImpl->config = config;
    }

    Game::~Game() = default;
    Game::Game(Game&&) noexcept = default;
    Game& Game::operator=(Game&&) noexcept = default;

    Game& Game::setTitle(const std::string& title) {
        m_pImpl->config.title = title;
        return *this;
    }

    Game& Game::setSize(int width, int height) {
        m_pImpl->config.width = width;
        m_pImpl->config.height = height;
        return *this;
    }

    Game& Game::setVSync(bool enabled) {
        m_pImpl->config.vsync = enabled;
        return *this;
    }

    graphics::Mesh* Game::createCube(float size) {
        auto data = graphics::generateCube(size);
        auto mesh = std::make_unique<graphics::Mesh>();
        mesh->setVertices(std::span<const graphics::VertexPositionNormalUV>(data.vertices));
        mesh->setIndices(std::span<const uint32_t>(data.indices));

        if (m_pImpl->initialized && m_pImpl->renderer) {
            auto* shaderRegistry = m_pImpl->renderer->getShaderRegistry();
            mesh->createGPUResources(m_pImpl->renderer->getDevice(), shaderRegistry->getModelBindGroupLayout());
        }

        auto* ptr = mesh.get();
        m_pImpl->ownedMeshes.push_back(std::move(mesh));
        return ptr;
    }

    graphics::Mesh* Game::createSphere(float radius, int segments) {
        auto data = graphics::generateSphere(radius, segments, segments);
        auto mesh = std::make_unique<graphics::Mesh>();
        mesh->setVertices(std::span<const graphics::VertexPositionNormalUV>(data.vertices));
        mesh->setIndices(std::span<const uint32_t>(data.indices));

        if (m_pImpl->initialized && m_pImpl->renderer) {
            auto* shaderRegistry = m_pImpl->renderer->getShaderRegistry();
            mesh->createGPUResources(m_pImpl->renderer->getDevice(), shaderRegistry->getModelBindGroupLayout());
        }

        auto* ptr = mesh.get();
        m_pImpl->ownedMeshes.push_back(std::move(mesh));
        return ptr;
    }

    graphics::Mesh* Game::createPlane(float width, float height) {
        auto data = graphics::generatePlane(width, height);
        auto mesh = std::make_unique<graphics::Mesh>();
        mesh->setVertices(std::span<const graphics::VertexPositionNormalUV>(data.vertices));
        mesh->setIndices(std::span<const uint32_t>(data.indices));

        if (m_pImpl->initialized && m_pImpl->renderer) {
            auto* shaderRegistry = m_pImpl->renderer->getShaderRegistry();
            mesh->createGPUResources(m_pImpl->renderer->getDevice(), shaderRegistry->getModelBindGroupLayout());
        }

        auto* ptr = mesh.get();
        m_pImpl->ownedMeshes.push_back(std::move(mesh));
        return ptr;
    }

    graphics::Mesh* Game::createQuad(float width, float height) {
        auto data = graphics::generateQuad(width, height);
        auto mesh = std::make_unique<graphics::Mesh>();
        mesh->setVertices(std::span<const graphics::VertexPositionNormalUV>(data.vertices));
        mesh->setIndices(std::span<const uint32_t>(data.indices));

        if (m_pImpl->initialized && m_pImpl->renderer) {
            auto* shaderRegistry = m_pImpl->renderer->getShaderRegistry();
            mesh->createGPUResources(m_pImpl->renderer->getDevice(), shaderRegistry->getModelBindGroupLayout());
        }

        auto* ptr = mesh.get();
        m_pImpl->ownedMeshes.push_back(std::move(mesh));
        return ptr;
    }

    graphics::Material* Game::createMaterial() {
        auto material = graphics::Material::createUnlit(glm::vec4(1.0f));

        if (m_pImpl->initialized && m_pImpl->renderer) {
            auto* shaderRegistry = m_pImpl->renderer->getShaderRegistry();
            auto layout = shaderRegistry->getMaterialBindGroupLayout(material->getShader());
            material->createGPUResources(m_pImpl->renderer->getDevice(), layout);
        }

        auto* ptr = material.get();
        m_pImpl->ownedMaterials.push_back(std::move(material));
        return ptr;
    }

    graphics::Material* Game::createUnlitMaterial(const glm::vec4& color) {
        auto material = graphics::Material::createUnlit(color);

        if (m_pImpl->initialized && m_pImpl->renderer) {
            auto* shaderRegistry = m_pImpl->renderer->getShaderRegistry();
            auto layout = shaderRegistry->getMaterialBindGroupLayout(material->getShader());
            material->createGPUResources(m_pImpl->renderer->getDevice(), layout);
        }

        auto* ptr = material.get();
        m_pImpl->ownedMaterials.push_back(std::move(material));
        return ptr;
    }

    graphics::Material* Game::createSpriteMaterial(graphics::GPUTexture* texture, const glm::vec4& tint) {
        auto material = graphics::Material::createSprite(texture, tint);

        if (m_pImpl->initialized && m_pImpl->renderer) {
            auto* shaderRegistry = m_pImpl->renderer->getShaderRegistry();
            auto layout = shaderRegistry->getMaterialBindGroupLayout(material->getShader());
            material->createGPUResources(m_pImpl->renderer->getDevice(), layout);
        }

        auto* ptr = material.get();
        m_pImpl->ownedMaterials.push_back(std::move(material));
        return ptr;
    }

    graphics::Material* Game::createGlowMaterial(const glm::vec4& color) {
        auto material = graphics::Material::createGlow(color);

        if (m_pImpl->initialized && m_pImpl->renderer) {
            auto* shaderRegistry = m_pImpl->renderer->getShaderRegistry();
            auto layout = shaderRegistry->getMaterialBindGroupLayout(material->getShader());
            material->createGPUResources(m_pImpl->renderer->getDevice(), layout);
        }

        auto* ptr = material.get();
        m_pImpl->ownedMaterials.push_back(std::move(material));
        return ptr;
    }

    graphics::PBRMaterial* Game::createPBRMaterial() {
        auto material = std::make_unique<graphics::PBRMaterial>();
        auto* ptr = material.get();
        m_pImpl->ownedPBRMaterials.push_back(std::move(material));
        return ptr;
    }

    graphics::GlassMaterial* Game::createGlassMaterial() {
        auto material = std::make_unique<graphics::GlassMaterial>();
        auto* ptr = material.get();
        m_pImpl->ownedGlassMaterials.push_back(std::move(material));
        return ptr;
    }

    graphics::GPUTexture* Game::loadTexture(const std::string& path) {
        if (!m_pImpl->initialized || !m_pImpl->renderer) {
            spdlog::error("Cannot load texture before game is initialized");
            return nullptr;
        }

        auto texture = graphics::TextureLoader::load(m_pImpl->renderer->getDevice(), path);
        if (!texture) {
            spdlog::error("Failed to load texture: {}", path);
            return nullptr;
        }

        auto* ptr = texture.get();
        m_pImpl->ownedTextures.push_back(std::move(texture));
        return ptr;
    }

    void Game::addMesh(graphics::Mesh* mesh) {
        if (m_pImpl->renderer) {
            m_pImpl->renderer->addMesh(mesh);
        }
    }

    void Game::removeMesh(graphics::Mesh* mesh) {
        if (m_pImpl->renderer) {
            m_pImpl->renderer->removeMesh(mesh);
        }
    }

    void Game::clearMeshes() {
        if (m_pImpl->renderer) {
            m_pImpl->renderer->clearMeshes();
        }
    }

    void Game::setCameraPosition(const glm::vec3& position) {
        m_pImpl->cameraPosition = position;
        m_pImpl->cameraDirty = true;
    }

    void Game::setCameraLookAt(const glm::vec3& target) {
        m_pImpl->cameraTarget = target;
        m_pImpl->cameraDirty = true;
    }

    void Game::setCameraUp(const glm::vec3& up) {
        m_pImpl->cameraUp = up;
        m_pImpl->cameraDirty = true;
    }

    void Game::setPerspective(float fovDegrees, float nearPlane, float farPlane) {
        m_pImpl->fov = fovDegrees;
        m_pImpl->nearPlane = nearPlane;
        m_pImpl->farPlane = farPlane;
        m_pImpl->usePerspective = true;
        m_pImpl->cameraDirty = true;
    }

    void Game::setOrthographic(float size, float nearPlane, float farPlane) {
        m_pImpl->orthoSize = size;
        m_pImpl->nearPlane = nearPlane;
        m_pImpl->farPlane = farPlane;
        m_pImpl->usePerspective = false;
        m_pImpl->cameraDirty = true;
    }

    void Game::setAmbientLight(const glm::vec3& color, float intensity) {
        if (!m_pImpl->lighting) {
            m_pImpl->lighting = std::make_unique<graphics::LightingManager>();
        }
        m_pImpl->lighting->setAmbient(color, intensity);
    }

    void Game::addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity) {
        if (!m_pImpl->lighting) {
            m_pImpl->lighting = std::make_unique<graphics::LightingManager>();
        }
        m_pImpl->lighting->addDirectionalLight(direction, color, intensity);
    }

    void Game::addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float range) {
        if (!m_pImpl->lighting) {
            m_pImpl->lighting = std::make_unique<graphics::LightingManager>();
        }
        m_pImpl->lighting->addPointLight(position, color, intensity, range);
    }

    void Game::clearLights() {
        if (m_pImpl->lighting) {
            m_pImpl->lighting->clearLights();
        }
    }

    graphics::LightingManager* Game::getLighting() {
        return m_pImpl->lighting.get();
    }

    audio::AudioManager* Game::getAudio() {
        return m_pImpl->audioManager.get();
    }

    audio::AudioClip* Game::loadAudio(const std::string& path) {
        if (!m_pImpl->audioManager) return nullptr;
        return m_pImpl->audioManager->loadClip(path);
    }

    void Game::playSound(audio::AudioClip* clip, float volume) {
        if (m_pImpl->audioManager && clip) {
            m_pImpl->audioManager->play(clip, volume);
        }
    }

    void Game::playSound(const std::string& path, float volume) {
        if (m_pImpl->audioManager) {
            m_pImpl->audioManager->playOneShot(path, volume);
        }
    }

    bool Game::isKeyPressed(int scancode) const {
        const bool* keyState = SDL_GetKeyboardState(nullptr);
        return keyState && keyState[scancode];
    }

    void Game::onUpdate(UpdateCallback callback) {
        m_pImpl->updateCallback = std::move(callback);
    }

    flecs::world& Game::world() {
        return m_pImpl->world;
    }

    graphics::WGPURenderer* Game::getRenderer() {
        return m_pImpl->renderer.get();
    }

    core::Result Game::initialize() {
        spdlog::info("Initializing Game: {}", m_pImpl->config.title);

        m_pImpl->window = std::make_unique<infrastructure::Window>();
        infrastructure::WindowConfig windowConfig;
        windowConfig.title = m_pImpl->config.title;
        windowConfig.width = m_pImpl->config.width;
        windowConfig.height = m_pImpl->config.height;
        windowConfig.vsync = m_pImpl->config.vsync;

        if (m_pImpl->window->initialize(windowConfig) != core::Result::Success) {
            spdlog::error("Failed to initialize window");
            return core::Result::Error;
        }

        m_pImpl->renderer = std::make_unique<graphics::WGPURenderer>();
        if (m_pImpl->renderer->initialize(m_pImpl->window.get()) != core::Result::Success) {
            spdlog::error("Failed to initialize renderer");
            return core::Result::Error;
        }

        m_pImpl->audioManager = std::make_unique<audio::AudioManager>();
        if (m_pImpl->audioManager->initialize() != core::Result::Success) {
            spdlog::warn("Failed to initialize audio (continuing without audio)");
        }

        auto* shaderRegistry = m_pImpl->renderer->getShaderRegistry();
        auto modelLayout = shaderRegistry->getModelBindGroupLayout();

        for (auto& mesh : m_pImpl->ownedMeshes) {
            mesh->createGPUResources(m_pImpl->renderer->getDevice(), modelLayout);
            m_pImpl->renderer->addMesh(mesh.get());
        }

        for (auto& material : m_pImpl->ownedMaterials) {
            auto* sr = m_pImpl->renderer->getShaderRegistry();
            auto layout = sr->getMaterialBindGroupLayout(material->getShader());
            material->createGPUResources(m_pImpl->renderer->getDevice(), layout);
        }

        m_pImpl->updateCamera();

        m_pImpl->initialized = true;
        spdlog::info("Game initialized successfully");
        return core::Result::Success;
    }

    void Game::run() {
        if (!m_pImpl->initialized) {
            if (initialize() != core::Result::Success) {
                spdlog::error("Failed to initialize game");
                return;
            }
        }

        spdlog::info("Starting game loop");

        while (!m_pImpl->window->shouldClose()) {
            float currentTime = static_cast<float>(m_pImpl->window->getTime());
            float deltaTime = currentTime - m_pImpl->lastFrameTime;
            m_pImpl->lastFrameTime = currentTime;

            m_pImpl->window->pollEvents();

            m_pImpl->world.progress(deltaTime);

            if (m_pImpl->updateCallback) {
                m_pImpl->updateCallback(deltaTime);
            }

            m_pImpl->updateCamera();

            m_pImpl->renderer->render();
        }

        spdlog::info("Game loop ended");
    }

}
