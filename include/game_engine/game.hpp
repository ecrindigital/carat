#pragma once

#include <game_engine/core/types.hpp>
#include <game_engine/core/system.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <functional>

namespace game_engine {

    namespace graphics {
        class Mesh;
        class Material;
        class PBRMaterial;
        class GlassMaterial;
        class GPUTexture;
        class WGPURenderer;
        class LightingManager;
    }

    namespace infrastructure {
        class EcsManager;
    }

    struct GameConfig {
        std::string title = "Carat Engine";
        int width = 800;
        int height = 600;
        bool vsync = true;
    };

    class Game {
    public:
        Game();
        Game(const std::string& title, int width, int height);
        explicit Game(const GameConfig& config);
        ~Game();

        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;
        Game(Game&&) noexcept;
        Game& operator=(Game&&) noexcept;

        Game& setTitle(const std::string& title);
        Game& setSize(int width, int height);
        Game& setVSync(bool enabled);

        template<typename T>
        Game& addSystem();

        graphics::Mesh* createCube(float size = 1.0f);
        graphics::Mesh* createSphere(float radius = 0.5f, int segments = 32);
        graphics::Mesh* createPlane(float width = 1.0f, float height = 1.0f);

        graphics::Material* createMaterial();
        graphics::Material* createUnlitMaterial(const glm::vec4& color);
        graphics::PBRMaterial* createPBRMaterial();
        graphics::GlassMaterial* createGlassMaterial();

        void addMesh(graphics::Mesh* mesh);
        void removeMesh(graphics::Mesh* mesh);
        void clearMeshes();

        void setCameraPosition(const glm::vec3& position);
        void setCameraLookAt(const glm::vec3& target);
        void setCameraUp(const glm::vec3& up);
        void setPerspective(float fovDegrees, float nearPlane, float farPlane);
        void setOrthographic(float size, float nearPlane, float farPlane);

        void setAmbientLight(const glm::vec3& color, float intensity);
        void addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity = 1.0f);
        void addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity = 1.0f, float range = 10.0f);
        void clearLights();

        using UpdateCallback = std::function<void(float deltaTime)>;
        void onUpdate(UpdateCallback callback);

        infrastructure::EcsManager& ecs();
        graphics::WGPURenderer* getRenderer();
        graphics::LightingManager* getLighting();

        core::Result initialize();
        void run();

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;

        template<typename T>
        void registerSystemInternal();
    };

    template<typename T>
    Game& Game::addSystem() {
        registerSystemInternal<T>();
        return *this;
    }

}
