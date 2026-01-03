#pragma once

#include <game_engine/core/types.hpp>
#include <game_engine/graphics/builtin_shaders.hpp>
#include <memory>

typedef struct WGPUBindGroupLayoutImpl* WGPUBindGroupLayout;

namespace game_engine::graphics {

    class GPUDevice;
    class GPUPipeline;

    class ShaderRegistry {
    public:
        ShaderRegistry();
        ~ShaderRegistry();

        ShaderRegistry(const ShaderRegistry&) = delete;
        ShaderRegistry& operator=(const ShaderRegistry&) = delete;
        ShaderRegistry(ShaderRegistry&&) noexcept;
        ShaderRegistry& operator=(ShaderRegistry&&) noexcept;

        [[nodiscard]] core::Result initialize(GPUDevice* device);
        void shutdown();

        [[nodiscard]] GPUPipeline* getPipeline(BuiltinShader shader);

        [[nodiscard]] WGPUBindGroupLayout getCameraBindGroupLayout() const;
        [[nodiscard]] WGPUBindGroupLayout getModelBindGroupLayout() const;
        [[nodiscard]] WGPUBindGroupLayout getMaterialBindGroupLayout(BuiltinShader shader) const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

} // namespace game_engine::graphics
