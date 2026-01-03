#pragma once

#include <game_engine/core/types.hpp>
#include <game_engine/graphics/vertex_layout.hpp>
#include <game_engine/graphics/builtin_shaders.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <span>
#include <vector>

namespace game_engine::graphics {

    class GPUBuffer;
    class GPUBindGroup;
    class GPUDevice;
    class GPUTexture;
    class Material;

    class Mesh {
    public:
        Mesh();
        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) noexcept;
        Mesh& operator=(Mesh&&) noexcept;

        void setVertices(std::span<const VertexPosition> vertices);
        void setVertices(std::span<const VertexPositionColor> vertices);
        void setVertices(std::span<const VertexPositionUV> vertices);
        void setVertices(std::span<const VertexPositionNormalUV> vertices);
        void setVerticesRaw(const void* data, size_t size, VertexLayoutType layout);

        void setIndices(std::span<const uint16_t> indices);
        void setIndices(std::span<const uint32_t> indices);

        void setMaterial(Material* material);
        [[nodiscard]] Material* getMaterial() const;

        void setShader(BuiltinShader shader);
        void setColor(const glm::vec4& color);
        void setTexture(GPUTexture* texture);

        void setPosition(const glm::vec3& position);
        [[nodiscard]] glm::vec3 getPosition() const;

        void setRotation(const glm::vec3& rotation);
        [[nodiscard]] glm::vec3 getRotation() const;

        void setScale(const glm::vec3& scale);
        [[nodiscard]] glm::vec3 getScale() const;

        [[nodiscard]] glm::mat4 getModelMatrix() const;

        [[nodiscard]] core::Result createGPUResources(GPUDevice* device, void* modelBindGroupLayout);
        [[nodiscard]] GPUBuffer* getVertexBuffer() const;
        [[nodiscard]] GPUBindGroup* getModelBindGroup() const;
        [[nodiscard]] GPUBuffer* getIndexBuffer() const;
        [[nodiscard]] uint32_t getVertexCount() const;
        [[nodiscard]] uint32_t getIndexCount() const;
        [[nodiscard]] VertexLayoutType getVertexLayoutType() const;
        [[nodiscard]] bool hasIndices() const;
        [[nodiscard]] bool isIndexFormat32() const;

        [[nodiscard]] GPUBuffer* getModelUniformBuffer() const;
        void updateModelUniformBuffer();

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

}
