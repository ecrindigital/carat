#include <game_engine/graphics/pbr_material.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <game_engine/graphics/gpu_buffer.hpp>
#include <game_engine/graphics/gpu_bind_group.hpp>
#include <game_engine/graphics/gpu_texture.hpp>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    class PBRMaterial::Impl {
    public:
        PBRMaterialData data;
        std::unique_ptr<GPUBuffer> uniformBuffer;
        std::unique_ptr<GPUBindGroup> bindGroup;
        GPUTexture* albedoTexture = nullptr;
        GPUTexture* normalTexture = nullptr;
        GPUTexture* metallicRoughnessTexture = nullptr;
        GPUTexture* aoTexture = nullptr;
        GPUTexture* emissiveTexture = nullptr;
        bool dirty = true;
    };

    PBRMaterial::PBRMaterial() : m_pImpl(std::make_unique<Impl>()) {}
    PBRMaterial::~PBRMaterial() = default;
    PBRMaterial::PBRMaterial(PBRMaterial&&) noexcept = default;
    PBRMaterial& PBRMaterial::operator=(PBRMaterial&&) noexcept = default;

    void PBRMaterial::setAlbedo(const glm::vec4& color) {
        m_pImpl->data.albedo = color;
        m_pImpl->dirty = true;
    }

    void PBRMaterial::setAlbedo(const glm::vec3& color) {
        m_pImpl->data.albedo = glm::vec4(color, 1.0f);
        m_pImpl->dirty = true;
    }

    void PBRMaterial::setMetallic(float metallic) {
        m_pImpl->data.metallic = glm::clamp(metallic, 0.0f, 1.0f);
        m_pImpl->dirty = true;
    }

    void PBRMaterial::setRoughness(float roughness) {
        m_pImpl->data.roughness = glm::clamp(roughness, 0.04f, 1.0f);
        m_pImpl->dirty = true;
    }

    void PBRMaterial::setAO(float ao) {
        m_pImpl->data.ao = glm::clamp(ao, 0.0f, 1.0f);
        m_pImpl->dirty = true;
    }

    void PBRMaterial::setEmissive(const glm::vec3& color, float intensity) {
        m_pImpl->data.emissive = color;
        m_pImpl->data.emissiveIntensity = intensity;
        m_pImpl->dirty = true;
    }

    void PBRMaterial::setAlbedoTexture(GPUTexture* texture) {
        m_pImpl->albedoTexture = texture;
    }

    void PBRMaterial::setNormalTexture(GPUTexture* texture) {
        m_pImpl->normalTexture = texture;
    }

    void PBRMaterial::setMetallicRoughnessTexture(GPUTexture* texture) {
        m_pImpl->metallicRoughnessTexture = texture;
    }

    void PBRMaterial::setAOTexture(GPUTexture* texture) {
        m_pImpl->aoTexture = texture;
    }

    void PBRMaterial::setEmissiveTexture(GPUTexture* texture) {
        m_pImpl->emissiveTexture = texture;
    }

    const PBRMaterialData& PBRMaterial::getData() const {
        return m_pImpl->data;
    }

    core::Result PBRMaterial::createGPUResources(GPUDevice* device, WGPUBindGroupLayout layout) {
        m_pImpl->uniformBuffer = std::make_unique<GPUBuffer>();
        if (m_pImpl->uniformBuffer->initialize(device, sizeof(PBRMaterialData), BufferUsage::Uniform) != core::Result::Success) {
            spdlog::error("Failed to create PBR material uniform buffer");
            return core::Result::Error;
        }

        m_pImpl->uniformBuffer->write(&m_pImpl->data, sizeof(PBRMaterialData));

        std::vector<BindGroupEntry> entries = {
            {0, BindingType::UniformBuffer, m_pImpl->uniformBuffer.get(), 0, sizeof(PBRMaterialData), nullptr}
        };

        m_pImpl->bindGroup = std::make_unique<GPUBindGroup>();
        if (m_pImpl->bindGroup->initialize(device, layout, entries) != core::Result::Success) {
            spdlog::error("Failed to create PBR material bind group");
            return core::Result::Error;
        }

        m_pImpl->dirty = false;
        spdlog::info("PBRMaterial GPU resources created");
        return core::Result::Success;
    }

    void PBRMaterial::updateUniformBuffer() {
        if (m_pImpl->uniformBuffer && m_pImpl->dirty) {
            m_pImpl->uniformBuffer->write(&m_pImpl->data, sizeof(PBRMaterialData));
            m_pImpl->dirty = false;
        }
    }

    GPUBindGroup* PBRMaterial::getBindGroup() const {
        return m_pImpl->bindGroup.get();
    }

    void PBRMaterial::markDirty() {
        m_pImpl->dirty = true;
    }

    bool PBRMaterial::isDirty() const {
        return m_pImpl->dirty;
    }

}
