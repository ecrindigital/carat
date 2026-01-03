#pragma once

#include <game_engine/core/types.hpp>
#include <game_engine/graphics/vertex_layout.hpp>
#include <memory>
#include <string>
#include <vector>
#include <optional>

typedef struct WGPURenderPipelineImpl* WGPURenderPipeline;
typedef struct WGPUShaderModuleImpl* WGPUShaderModule;
typedef struct WGPUBindGroupLayoutImpl* WGPUBindGroupLayout;
typedef struct WGPUPipelineLayoutImpl* WGPUPipelineLayout;

namespace game_engine::graphics {

    class GPUDevice;

    struct ShaderSource {
        std::string code;
        std::string entryPoint = "main";
    };

    struct PipelineConfig {
        ShaderSource vertexShader;
        ShaderSource fragmentShader;
        std::optional<VertexLayoutType> vertexLayout;
        std::vector<WGPUBindGroupLayout> bindGroupLayouts;
        bool enableBlending = true;
        bool enableDepthTest = false;
    };

    class GPUPipeline {
    public:
        GPUPipeline();
        ~GPUPipeline();

        GPUPipeline(const GPUPipeline&) = delete;
        GPUPipeline& operator=(const GPUPipeline&) = delete;
        GPUPipeline(GPUPipeline&&) noexcept;
        GPUPipeline& operator=(GPUPipeline&&) noexcept;

        [[nodiscard]] core::Result initialize(GPUDevice* device, const PipelineConfig& config);
        void shutdown();

        [[nodiscard]] WGPURenderPipeline getPipeline() const;
        [[nodiscard]] WGPUPipelineLayout getPipelineLayout() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

}
