#include <game_engine/graphics/glass_material.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <game_engine/graphics/gpu_buffer.hpp>
#include <game_engine/graphics/gpu_bind_group.hpp>
#include <game_engine/graphics/gpu_texture.hpp>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    class GlassMaterial::Impl {
    public:
        GlassMaterialData data;
        std::unique_ptr<GPUBuffer> uniformBuffer;
        std::unique_ptr<GPUBindGroup> bindGroup;
        GPUTexture* environmentMap = nullptr;
        bool dirty = true;
    };

    GlassMaterial::GlassMaterial() : m_pImpl(std::make_unique<Impl>()) {}
    GlassMaterial::~GlassMaterial() = default;
    GlassMaterial::GlassMaterial(GlassMaterial&&) noexcept = default;
    GlassMaterial& GlassMaterial::operator=(GlassMaterial&&) noexcept = default;

    void GlassMaterial::setTint(const glm::vec4& tint) {
        m_pImpl->data.tint = tint;
        m_pImpl->dirty = true;
    }

    void GlassMaterial::setTint(const glm::vec3& color, float opacity) {
        m_pImpl->data.tint = glm::vec4(color, opacity);
        m_pImpl->dirty = true;
    }

    void GlassMaterial::setIOR(float ior) {
        m_pImpl->data.ior = glm::clamp(ior, 1.0f, 3.0f);
        m_pImpl->dirty = true;
    }

    void GlassMaterial::setRoughness(float roughness) {
        m_pImpl->data.roughness = glm::clamp(roughness, 0.0f, 1.0f);
        m_pImpl->dirty = true;
    }

    void GlassMaterial::setFresnelPower(float power) {
        m_pImpl->data.fresnelPower = glm::clamp(power, 1.0f, 10.0f);
        m_pImpl->dirty = true;
    }

    void GlassMaterial::setReflectivity(float reflectivity) {
        m_pImpl->data.reflectivity = glm::clamp(reflectivity, 0.0f, 1.0f);
        m_pImpl->dirty = true;
    }

    void GlassMaterial::setThickness(float thickness) {
        m_pImpl->data.thickness = glm::clamp(thickness, 0.0f, 1.0f);
        m_pImpl->dirty = true;
    }

    void GlassMaterial::setDispersion(float dispersion) {
        m_pImpl->data.dispersion = glm::clamp(dispersion, 0.0f, 0.1f);
        m_pImpl->dirty = true;
    }

    void GlassMaterial::setEnvironmentMap(GPUTexture* cubemap) {
        m_pImpl->environmentMap = cubemap;
    }

    const GlassMaterialData& GlassMaterial::getData() const {
        return m_pImpl->data;
    }

    core::Result GlassMaterial::createGPUResources(GPUDevice* device, WGPUBindGroupLayout layout) {
        m_pImpl->uniformBuffer = std::make_unique<GPUBuffer>();
        if (m_pImpl->uniformBuffer->initialize(device, sizeof(GlassMaterialData), BufferUsage::Uniform) != core::Result::Success) {
            spdlog::error("Failed to create glass material uniform buffer");
            return core::Result::Error;
        }

        m_pImpl->uniformBuffer->write(&m_pImpl->data, sizeof(GlassMaterialData));

        std::vector<BindGroupEntry> entries = {
            {0, BindingType::UniformBuffer, m_pImpl->uniformBuffer.get(), 0, sizeof(GlassMaterialData), nullptr}
        };

        m_pImpl->bindGroup = std::make_unique<GPUBindGroup>();
        if (m_pImpl->bindGroup->initialize(device, layout, entries) != core::Result::Success) {
            spdlog::error("Failed to create glass material bind group");
            return core::Result::Error;
        }

        m_pImpl->dirty = false;
        spdlog::info("GlassMaterial GPU resources created");
        return core::Result::Success;
    }

    void GlassMaterial::updateUniformBuffer() {
        if (m_pImpl->uniformBuffer && m_pImpl->dirty) {
            m_pImpl->uniformBuffer->write(&m_pImpl->data, sizeof(GlassMaterialData));
            m_pImpl->dirty = false;
        }
    }

    GPUBindGroup* GlassMaterial::getBindGroup() const {
        return m_pImpl->bindGroup.get();
    }

    void GlassMaterial::markDirty() {
        m_pImpl->dirty = true;
    }

    bool GlassMaterial::isDirty() const {
        return m_pImpl->dirty;
    }

}
