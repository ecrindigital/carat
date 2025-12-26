#pragma once

#include <game_engine/core/types.hpp>
#include <memory>
#include <string>

typedef struct WGPURenderPipelineImpl* WGPURenderPipeline;
typedef struct WGPUShaderModuleImpl* WGPUShaderModule;

namespace game_engine::graphics {

    class GPUDevice;

    struct ShaderSource {
        std::string code;
        std::string entryPoint = "main";
    };

    struct PipelineConfig {
        ShaderSource vertexShader;
        ShaderSource fragmentShader;
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

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

} // namespace game_engine::graphics
