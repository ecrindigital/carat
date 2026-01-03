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

    struct alignas(16) GlassMaterialData {
        glm::vec4 tint{1.0f, 1.0f, 1.0f, 0.1f};
        float ior{1.5f};
        float roughness{0.0f};
        float fresnelPower{5.0f};
        float reflectivity{0.04f};
        float thickness{0.1f};
        float dispersion{0.0f};
        float _pad1{0.0f};
        float _pad2{0.0f};
    };

    class GlassMaterial {
    public:
        GlassMaterial();
        ~GlassMaterial();

        GlassMaterial(const GlassMaterial&) = delete;
        GlassMaterial& operator=(const GlassMaterial&) = delete;
        GlassMaterial(GlassMaterial&&) noexcept;
        GlassMaterial& operator=(GlassMaterial&&) noexcept;

        void setTint(const glm::vec4& tint);
        void setTint(const glm::vec3& color, float opacity = 0.1f);
        void setIOR(float ior);
        void setRoughness(float roughness);
        void setFresnelPower(float power);
        void setReflectivity(float reflectivity);
        void setThickness(float thickness);
        void setDispersion(float dispersion);

        void setEnvironmentMap(GPUTexture* cubemap);

        [[nodiscard]] const GlassMaterialData& getData() const;

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
