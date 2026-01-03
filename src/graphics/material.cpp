#include <game_engine/graphics/material.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <game_engine/graphics/gpu_buffer.hpp>
#include <game_engine/graphics/gpu_texture.hpp>
#include <game_engine/graphics/gpu_bind_group.hpp>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    class Material::Impl {
    public:
        BuiltinShader shader = BuiltinShader::Unlit;
        MaterialData data;
        GPUTexture* texture = nullptr;

        std::unique_ptr<GPUBuffer> uniformBuffer;
        std::unique_ptr<GPUBindGroup> bindGroup;
        std::unique_ptr<GPUTexture> defaultTexture;

        GPUDevice* device = nullptr;
        bool dirty = true;
        bool gpuResourcesCreated = false;
    };

    Material::Material() : m_pImpl(std::make_unique<Impl>()) {}

    Material::~Material() = default;

    Material::Material(Material&&) noexcept = default;
    Material& Material::operator=(Material&&) noexcept = default;

    std::unique_ptr<Material> Material::createUnlit(const glm::vec4& color) {
        auto material = std::make_unique<Material>();
        material->m_pImpl->shader = BuiltinShader::Unlit;
        material->m_pImpl->data.color = color;
        return material;
    }

    std::unique_ptr<Material> Material::createUnlitTextured(GPUTexture* texture, const glm::vec4& tint) {
        auto material = std::make_unique<Material>();
        material->m_pImpl->shader = BuiltinShader::UnlitTextured;
        material->m_pImpl->data.color = tint;
        material->m_pImpl->texture = texture;
        return material;
    }

    std::unique_ptr<Material> Material::createSprite(GPUTexture* texture, const glm::vec4& tint) {
        auto material = std::make_unique<Material>();
        material->m_pImpl->shader = BuiltinShader::Sprite;
        material->m_pImpl->data.color = tint;
        material->m_pImpl->texture = texture;
        return material;
    }

    std::unique_ptr<Material> Material::createGlow(const glm::vec4& color) {
        auto material = std::make_unique<Material>();
        material->m_pImpl->shader = BuiltinShader::Glow;
        material->m_pImpl->data.color = color;
        return material;
    }

    void Material::setColor(const glm::vec4& color) {
        m_pImpl->data.color = color;
        markDirty();
    }

    glm::vec4 Material::getColor() const {
        return m_pImpl->data.color;
    }

    void Material::setTexture(GPUTexture* texture) {
        m_pImpl->texture = texture;
        m_pImpl->gpuResourcesCreated = false;
    }

    GPUTexture* Material::getTexture() const {
        return m_pImpl->texture;
    }

    void Material::setUVOffset(const glm::vec2& offset) {
        m_pImpl->data.uvOffset = offset;
        markDirty();
    }

    glm::vec2 Material::getUVOffset() const {
        return m_pImpl->data.uvOffset;
    }

    void Material::setUVScale(const glm::vec2& scale) {
        m_pImpl->data.uvScale = scale;
        markDirty();
    }

    glm::vec2 Material::getUVScale() const {
        return m_pImpl->data.uvScale;
    }

    void Material::setFlip(bool flipX, bool flipY) {
        m_pImpl->data.flipX = flipX ? 1 : 0;
        m_pImpl->data.flipY = flipY ? 1 : 0;
        markDirty();
    }

    bool Material::getFlipX() const {
        return m_pImpl->data.flipX != 0;
    }

    bool Material::getFlipY() const {
        return m_pImpl->data.flipY != 0;
    }

    BuiltinShader Material::getShader() const {
        return m_pImpl->shader;
    }

    const MaterialData& Material::getData() const {
        return m_pImpl->data;
    }

    core::Result Material::createGPUResources(GPUDevice* device, void* bindGroupLayout) {
        m_pImpl->device = device;

        auto layout = static_cast<WGPUBindGroupLayout>(bindGroupLayout);

        m_pImpl->uniformBuffer = std::make_unique<GPUBuffer>();
        if (m_pImpl->uniformBuffer->initialize(device, sizeof(MaterialData), BufferUsage::Uniform) != core::Result::Success) {
            spdlog::error("Failed to create material uniform buffer");
            return core::Result::Error;
        }

        m_pImpl->uniformBuffer->write(&m_pImpl->data, sizeof(MaterialData));

        std::vector<BindGroupEntry> entries;

        BindGroupEntry uniformEntry;
        uniformEntry.binding = 0;
        uniformEntry.type = BindingType::UniformBuffer;
        uniformEntry.buffer = m_pImpl->uniformBuffer.get();
        uniformEntry.size = sizeof(MaterialData);
        entries.push_back(uniformEntry);

        const auto& shaderDef = getShaderDefinition(m_pImpl->shader);
        if (shaderDef.hasTexture) {
            GPUTexture* tex = m_pImpl->texture;

            if (!tex) {
                m_pImpl->defaultTexture = std::make_unique<GPUTexture>();
                if (m_pImpl->defaultTexture->initializeDefault(device) != core::Result::Success) {
                    spdlog::error("Failed to create default texture");
                    return core::Result::Error;
                }
                tex = m_pImpl->defaultTexture.get();
            }

            BindGroupEntry textureEntry;
            textureEntry.binding = 1;
            textureEntry.type = BindingType::Texture;
            textureEntry.texture = tex;
            entries.push_back(textureEntry);

            BindGroupEntry samplerEntry;
            samplerEntry.binding = 2;
            samplerEntry.type = BindingType::Sampler;
            samplerEntry.texture = tex;
            entries.push_back(samplerEntry);
        }

        m_pImpl->bindGroup = std::make_unique<GPUBindGroup>();
        if (m_pImpl->bindGroup->initialize(device, layout, entries) != core::Result::Success) {
            spdlog::error("Failed to create material bind group");
            return core::Result::Error;
        }

        m_pImpl->gpuResourcesCreated = true;
        m_pImpl->dirty = false;

        return core::Result::Success;
    }

    GPUBindGroup* Material::getBindGroup() const {
        return m_pImpl->bindGroup.get();
    }

    GPUBuffer* Material::getUniformBuffer() const {
        return m_pImpl->uniformBuffer.get();
    }

    void Material::updateUniformBuffer() {
        if (m_pImpl->uniformBuffer && m_pImpl->dirty) {
            m_pImpl->uniformBuffer->write(&m_pImpl->data, sizeof(MaterialData));
            m_pImpl->dirty = false;
        }
    }

    void Material::markDirty() {
        m_pImpl->dirty = true;
    }

    bool Material::isDirty() const {
        return m_pImpl->dirty;
    }

}
