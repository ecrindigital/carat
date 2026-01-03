#include <game_engine/graphics/mesh.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <game_engine/graphics/gpu_buffer.hpp>
#include <game_engine/graphics/gpu_texture.hpp>
#include <game_engine/graphics/material.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    struct alignas(16) ModelData {
        glm::mat4 model{1.0f};
    };

    class Mesh::Impl {
    public:
        std::vector<uint8_t> vertexData;
        std::vector<uint8_t> indexData;
        VertexLayoutType vertexLayout = VertexLayoutType::Position;
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;
        bool indexFormat32 = false;

        std::unique_ptr<GPUBuffer> vertexBuffer;
        std::unique_ptr<GPUBuffer> indexBuffer;
        std::unique_ptr<GPUBuffer> modelUniformBuffer;

        Material* material = nullptr;
        std::unique_ptr<Material> ownedMaterial;

        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};
        ModelData modelData;
        bool transformDirty = true;

        void updateModelMatrix() {
            glm::mat4 model{1.0f};
            model = glm::translate(model, position);
            model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, scale);
            modelData.model = model;
        }
    };

    Mesh::Mesh() : m_pImpl(std::make_unique<Impl>()) {}

    Mesh::~Mesh() = default;

    Mesh::Mesh(Mesh&&) noexcept = default;
    Mesh& Mesh::operator=(Mesh&&) noexcept = default;

    void Mesh::setVertices(std::span<const VertexPosition> vertices) {
        setVerticesRaw(vertices.data(), vertices.size_bytes(), VertexLayoutType::Position);
        m_pImpl->vertexCount = static_cast<uint32_t>(vertices.size());
    }

    void Mesh::setVertices(std::span<const VertexPositionColor> vertices) {
        setVerticesRaw(vertices.data(), vertices.size_bytes(), VertexLayoutType::PositionColor);
        m_pImpl->vertexCount = static_cast<uint32_t>(vertices.size());
    }

    void Mesh::setVertices(std::span<const VertexPositionUV> vertices) {
        setVerticesRaw(vertices.data(), vertices.size_bytes(), VertexLayoutType::PositionUV);
        m_pImpl->vertexCount = static_cast<uint32_t>(vertices.size());
    }

    void Mesh::setVertices(std::span<const VertexPositionNormalUV> vertices) {
        setVerticesRaw(vertices.data(), vertices.size_bytes(), VertexLayoutType::PositionNormalUV);
        m_pImpl->vertexCount = static_cast<uint32_t>(vertices.size());
    }

    void Mesh::setVerticesRaw(const void* data, size_t size, VertexLayoutType layout) {
        m_pImpl->vertexData.resize(size);
        std::memcpy(m_pImpl->vertexData.data(), data, size);
        m_pImpl->vertexLayout = layout;
    }

    void Mesh::setIndices(std::span<const uint16_t> indices) {
        m_pImpl->indexData.resize(indices.size_bytes());
        std::memcpy(m_pImpl->indexData.data(), indices.data(), indices.size_bytes());
        m_pImpl->indexCount = static_cast<uint32_t>(indices.size());
        m_pImpl->indexFormat32 = false;
    }

    void Mesh::setIndices(std::span<const uint32_t> indices) {
        m_pImpl->indexData.resize(indices.size_bytes());
        std::memcpy(m_pImpl->indexData.data(), indices.data(), indices.size_bytes());
        m_pImpl->indexCount = static_cast<uint32_t>(indices.size());
        m_pImpl->indexFormat32 = true;
    }

    void Mesh::setMaterial(Material* material) {
        m_pImpl->material = material;
        m_pImpl->ownedMaterial.reset();
    }

    Material* Mesh::getMaterial() const {
        return m_pImpl->material;
    }

    void Mesh::setShader(BuiltinShader shader) {
        switch (shader) {
            case BuiltinShader::Unlit:
                m_pImpl->ownedMaterial = Material::createUnlit(glm::vec4(1.0f));
                break;
            case BuiltinShader::UnlitTextured:
                m_pImpl->ownedMaterial = Material::createUnlitTextured(nullptr);
                break;
            case BuiltinShader::Sprite:
                m_pImpl->ownedMaterial = Material::createSprite(nullptr);
                break;
            default:
                m_pImpl->ownedMaterial = Material::createUnlit(glm::vec4(1.0f));
                break;
        }
        m_pImpl->material = m_pImpl->ownedMaterial.get();
    }

    void Mesh::setColor(const glm::vec4& color) {
        if (m_pImpl->material) {
            m_pImpl->material->setColor(color);
        }
    }

    void Mesh::setTexture(GPUTexture* texture) {
        if (m_pImpl->material) {
            m_pImpl->material->setTexture(texture);
        }
    }

    void Mesh::setPosition(const glm::vec3& position) {
        m_pImpl->position = position;
        m_pImpl->transformDirty = true;
    }

    glm::vec3 Mesh::getPosition() const {
        return m_pImpl->position;
    }

    void Mesh::setRotation(const glm::vec3& rotation) {
        m_pImpl->rotation = rotation;
        m_pImpl->transformDirty = true;
    }

    glm::vec3 Mesh::getRotation() const {
        return m_pImpl->rotation;
    }

    void Mesh::setScale(const glm::vec3& scale) {
        m_pImpl->scale = scale;
        m_pImpl->transformDirty = true;
    }

    glm::vec3 Mesh::getScale() const {
        return m_pImpl->scale;
    }

    glm::mat4 Mesh::getModelMatrix() const {
        if (m_pImpl->transformDirty) {
            m_pImpl->updateModelMatrix();
            m_pImpl->transformDirty = false;
        }
        return m_pImpl->modelData.model;
    }

    core::Result Mesh::createGPUResources(GPUDevice* device) {
        if (m_pImpl->vertexData.empty()) {
            spdlog::error("No vertex data to upload");
            return core::Result::Error;
        }

        m_pImpl->vertexBuffer = std::make_unique<GPUBuffer>();
        if (m_pImpl->vertexBuffer->initialize(device, m_pImpl->vertexData.size(), BufferUsage::Vertex) != core::Result::Success) {
            spdlog::error("Failed to create vertex buffer");
            return core::Result::Error;
        }
        m_pImpl->vertexBuffer->write(m_pImpl->vertexData.data(), m_pImpl->vertexData.size());

        if (!m_pImpl->indexData.empty()) {
            m_pImpl->indexBuffer = std::make_unique<GPUBuffer>();
            if (m_pImpl->indexBuffer->initialize(device, m_pImpl->indexData.size(), BufferUsage::Index) != core::Result::Success) {
                spdlog::error("Failed to create index buffer");
                return core::Result::Error;
            }
            m_pImpl->indexBuffer->write(m_pImpl->indexData.data(), m_pImpl->indexData.size());
        }

        m_pImpl->modelUniformBuffer = std::make_unique<GPUBuffer>();
        if (m_pImpl->modelUniformBuffer->initialize(device, sizeof(ModelData), BufferUsage::Uniform) != core::Result::Success) {
            spdlog::error("Failed to create model uniform buffer");
            return core::Result::Error;
        }

        m_pImpl->updateModelMatrix();
        m_pImpl->modelUniformBuffer->write(&m_pImpl->modelData, sizeof(ModelData));

        return core::Result::Success;
    }

    GPUBuffer* Mesh::getVertexBuffer() const {
        return m_pImpl->vertexBuffer.get();
    }

    GPUBuffer* Mesh::getIndexBuffer() const {
        return m_pImpl->indexBuffer.get();
    }

    uint32_t Mesh::getVertexCount() const {
        return m_pImpl->vertexCount;
    }

    uint32_t Mesh::getIndexCount() const {
        return m_pImpl->indexCount;
    }

    VertexLayoutType Mesh::getVertexLayoutType() const {
        return m_pImpl->vertexLayout;
    }

    bool Mesh::hasIndices() const {
        return m_pImpl->indexCount > 0;
    }

    bool Mesh::isIndexFormat32() const {
        return m_pImpl->indexFormat32;
    }

    GPUBuffer* Mesh::getModelUniformBuffer() const {
        return m_pImpl->modelUniformBuffer.get();
    }

    void Mesh::updateModelUniformBuffer() {
        if (m_pImpl->transformDirty && m_pImpl->modelUniformBuffer) {
            m_pImpl->updateModelMatrix();
            m_pImpl->modelUniformBuffer->write(&m_pImpl->modelData, sizeof(ModelData));
            m_pImpl->transformDirty = false;
        }
    }

}
