#pragma once

#include <game_engine/core/types.hpp>
#include <glm/glm.hpp>
#include <memory>

typedef struct WGPUBindGroupLayoutImpl* WGPUBindGroupLayout;

namespace game_engine::graphics {

    class GPUDevice;
    class GPUBindGroup;
    class GPUBuffer;
    class GPUTexture;

    struct alignas(16) PBRMaterialData {
        glm::vec4 albedo{1.0f, 1.0f, 1.0f, 1.0f};
        float metallic{0.0f};
        float roughness{0.5f};
        float ao{1.0f};
        float _pad1{0.0f};
        glm::vec3 emissive{0.0f};
        float emissiveIntensity{0.0f};
    };

    class PBRMaterial {
    public:
        PBRMaterial();
        ~PBRMaterial();

        PBRMaterial(const PBRMaterial&) = delete;
        PBRMaterial& operator=(const PBRMaterial&) = delete;
        PBRMaterial(PBRMaterial&&) noexcept;
        PBRMaterial& operator=(PBRMaterial&&) noexcept;

        void setAlbedo(const glm::vec4& color);
        void setAlbedo(const glm::vec3& color);
        void setMetallic(float metallic);
        void setRoughness(float roughness);
        void setAO(float ao);
        void setEmissive(const glm::vec3& color, float intensity = 1.0f);

        void setAlbedoTexture(GPUTexture* texture);
        void setNormalTexture(GPUTexture* texture);
        void setMetallicRoughnessTexture(GPUTexture* texture);
        void setAOTexture(GPUTexture* texture);
        void setEmissiveTexture(GPUTexture* texture);

        [[nodiscard]] const PBRMaterialData& getData() const;

        [[nodiscard]] core::Result createGPUResources(GPUDevice* device, WGPUBindGroupLayout layout);
        void updateUniformBuffer();
        [[nodiscard]] GPUBindGroup* getBindGroup() const;

        void markDirty();
        [[nodiscard]] bool isDirty() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

}
