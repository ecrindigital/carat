#include <game_engine/graphics/lighting.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    class LightingManager::Impl {
    public:
        LightingData data;
        std::unique_ptr<GPUBuffer> uniformBuffer;
        bool dirty = true;
    };

    LightingManager::LightingManager() : m_pImpl(std::make_unique<Impl>()) {}
    LightingManager::~LightingManager() = default;
    LightingManager::LightingManager(LightingManager&&) noexcept = default;
    LightingManager& LightingManager::operator=(LightingManager&&) noexcept = default;

    void LightingManager::setAmbient(const glm::vec3& color, float intensity) {
        m_pImpl->data.ambientColor = glm::vec4(color * intensity, 1.0f);
        m_pImpl->dirty = true;
    }

    void LightingManager::setCameraPosition(const glm::vec3& position) {
        m_pImpl->data.cameraPosition = position;
        m_pImpl->dirty = true;
    }

    void LightingManager::addDirectionalLight(const glm::vec3& direction, const glm::vec3& color, float intensity) {
        if (m_pImpl->data.numDirectionalLights >= 4) {
            spdlog::warn("Maximum directional lights (4) reached");
            return;
        }

        auto& light = m_pImpl->data.directionalLights[m_pImpl->data.numDirectionalLights];
        light.direction = glm::normalize(direction);
        light.color = color;
        light.intensity = intensity;
        m_pImpl->data.numDirectionalLights++;
        m_pImpl->dirty = true;
    }

    int LightingManager::addPointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float range) {
        if (m_pImpl->data.numPointLights >= 8) {
            spdlog::warn("Maximum point lights (8) reached");
            return -1;
        }

        int index = static_cast<int>(m_pImpl->data.numPointLights);
        auto& light = m_pImpl->data.pointLights[index];
        light.position = position;
        light.color = color;
        light.intensity = intensity;
        light.range = range;
        m_pImpl->data.numPointLights++;
        m_pImpl->dirty = true;
        return index;
    }

    void LightingManager::updatePointLight(int index, const glm::vec3& position) {
        if (index < 0 || index >= static_cast<int>(m_pImpl->data.numPointLights)) return;
        m_pImpl->data.pointLights[index].position = position;
        m_pImpl->dirty = true;
    }

    void LightingManager::updatePointLight(int index, const glm::vec3& position, const glm::vec3& color, float intensity, float range) {
        if (index < 0 || index >= static_cast<int>(m_pImpl->data.numPointLights)) return;
        auto& light = m_pImpl->data.pointLights[index];
        light.position = position;
        light.color = color;
        light.intensity = intensity;
        light.range = range;
        m_pImpl->dirty = true;
    }

    void LightingManager::removePointLight(int index) {
        if (index < 0 || index >= static_cast<int>(m_pImpl->data.numPointLights)) return;
        for (uint32_t i = static_cast<uint32_t>(index); i < m_pImpl->data.numPointLights - 1; ++i) {
            m_pImpl->data.pointLights[i] = m_pImpl->data.pointLights[i + 1];
        }
        m_pImpl->data.numPointLights--;
        m_pImpl->dirty = true;
    }

    void LightingManager::clearLights() {
        m_pImpl->data.numDirectionalLights = 0;
        m_pImpl->data.numPointLights = 0;
        m_pImpl->dirty = true;
    }

    const LightingData& LightingManager::getData() const {
        return m_pImpl->data;
    }

    core::Result LightingManager::createGPUResources(GPUDevice* device) {
        m_pImpl->uniformBuffer = std::make_unique<GPUBuffer>();
        if (m_pImpl->uniformBuffer->initialize(device, sizeof(LightingData), BufferUsage::Uniform) != core::Result::Success) {
            spdlog::error("Failed to create lighting uniform buffer");
            return core::Result::Error;
        }

        m_pImpl->uniformBuffer->write(&m_pImpl->data, sizeof(LightingData));
        m_pImpl->dirty = false;

        spdlog::info("LightingManager GPU resources created");
        return core::Result::Success;
    }

    void LightingManager::updateUniformBuffer() {
        if (m_pImpl->uniformBuffer && m_pImpl->dirty) {
            m_pImpl->uniformBuffer->write(&m_pImpl->data, sizeof(LightingData));
            m_pImpl->dirty = false;
        }
    }

    GPUBuffer* LightingManager::getUniformBuffer() const {
        return m_pImpl->uniformBuffer.get();
    }

    void LightingManager::markDirty() {
        m_pImpl->dirty = true;
    }

    bool LightingManager::isDirty() const {
        return m_pImpl->dirty;
    }

}
