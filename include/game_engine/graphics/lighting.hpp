#pragma once

#include <game_engine/core/types.hpp>
#include <game_engine/graphics/gpu_buffer.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace game_engine::graphics {

    class GPUDevice;
    class GPUBindGroup;

    struct DirectionalLight {
        glm::vec3 direction{0.0f, -1.0f, 0.0f};
        float _pad1{0.0f};
        glm::vec3 color{1.0f, 1.0f, 1.0f};
        float intensity{1.0f};
    };

    struct PointLight {
        glm::vec3 position{0.0f};
        float range{10.0f};
        glm::vec3 color{1.0f, 1.0f, 1.0f};
        float intensity{1.0f};
    };

    struct alignas(16) LightingData {
        glm::vec4 ambientColor{0.1f, 0.1f, 0.1f, 1.0f};
        glm::vec3 cameraPosition{0.0f};
        float _pad1{0.0f};
        DirectionalLight directionalLights[4];
        PointLight pointLights[8];
        uint32_t numDirectionalLights{0};
        uint32_t numPointLights{0};
        float _pad2[2]{0.0f, 0.0f};
    };

    class LightingManager {
    public:
        LightingManager();
        ~LightingManager();

        LightingManager(const LightingManager&) = delete;
        LightingManager& operator=(const LightingManager&) = delete;
        LightingManager(LightingManager&&) noexcept;
        LightingManager& operator=(LightingManager&&) noexcept;

        void setAmbient(const glm::vec3& color, float intensity = 1.0f);
        void setCameraPosition(const glm::vec3& position);

        void addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity = 1.0f);
        int addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity = 1.0f, float range = 10.0f);
        void updatePointLight(int index, const glm::vec3& position);
        void updatePointLight(int index, const glm::vec3& position, const glm::vec3& color, float intensity, float range);
        void removePointLight(int index);
        void clearLights();

        [[nodiscard]] const LightingData& getData() const;

        [[nodiscard]] core::Result createGPUResources(GPUDevice* device);
        void updateUniformBuffer();
        [[nodiscard]] GPUBuffer* getUniformBuffer() const;

        void markDirty();
        [[nodiscard]] bool isDirty() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

}
