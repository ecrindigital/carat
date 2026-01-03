#pragma once

#include <game_engine/core/types.hpp>
#include <game_engine/graphics/builtin_shaders.hpp>
#include <glm/glm.hpp>
#include <memory>

namespace game_engine::graphics {

    class GPUTexture;
    class GPUDevice;
    class GPUBindGroup;
    class GPUBuffer;

    struct alignas(16) MaterialData {
        glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
        glm::vec2 uvOffset{0.0f, 0.0f};
        glm::vec2 uvScale{1.0f, 1.0f};
        uint32_t flipX{0};
        uint32_t flipY{0};
        float _padding[2]{0.0f, 0.0f};
    };

    class Material {
    public:
        Material();
        ~Material();

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
        Material(Material&&) noexcept;
        Material& operator=(Material&&) noexcept;

        static std::unique_ptr<Material> createUnlit(const glm::vec4& color);
        static std::unique_ptr<Material> createUnlitTextured(GPUTexture* texture, const glm::vec4& tint = glm::vec4(1.0f));
        static std::unique_ptr<Material> createSprite(GPUTexture* texture, const glm::vec4& tint = glm::vec4(1.0f));

        void setColor(const glm::vec4& color);
        [[nodiscard]] glm::vec4 getColor() const;

        void setTexture(GPUTexture* texture);
        [[nodiscard]] GPUTexture* getTexture() const;

        void setUVOffset(const glm::vec2& offset);
        [[nodiscard]] glm::vec2 getUVOffset() const;

        void setUVScale(const glm::vec2& scale);
        [[nodiscard]] glm::vec2 getUVScale() const;

        void setFlip(bool flipX, bool flipY);
        [[nodiscard]] bool getFlipX() const;
        [[nodiscard]] bool getFlipY() const;

        [[nodiscard]] BuiltinShader getShader() const;
        [[nodiscard]] const MaterialData& getData() const;

        [[nodiscard]] core::Result createGPUResources(GPUDevice* device, void* bindGroupLayout);
        [[nodiscard]] GPUBindGroup* getBindGroup() const;
        [[nodiscard]] GPUBuffer* getUniformBuffer() const;
        void updateUniformBuffer();

        void markDirty();
        [[nodiscard]] bool isDirty() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

}
