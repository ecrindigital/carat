#include <game_engine/graphics/shader_registry.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <game_engine/graphics/gpu_pipeline.hpp>
#include <game_engine/graphics/gpu_bind_group.hpp>
#include <game_engine/graphics/builtin_shaders.hpp>
#include <webgpu/webgpu.h>
#include <spdlog/spdlog.h>
#include <unordered_map>

namespace game_engine::graphics {

    class ShaderRegistry::Impl {
    public:
        GPUDevice* device = nullptr;
        std::unordered_map<BuiltinShader, std::unique_ptr<GPUPipeline>> pipelines;
        WGPUBindGroupLayout cameraBindGroupLayout = nullptr;
        WGPUBindGroupLayout modelBindGroupLayout = nullptr;
        std::unordered_map<BuiltinShader, WGPUBindGroupLayout> materialBindGroupLayouts;

        void createBindGroupLayouts() {
            {
                BindGroupLayoutBuilder builder;
                builder.addUniformBuffer(0, true, false);
                cameraBindGroupLayout = builder.build(device);
            }

            {
                BindGroupLayoutBuilder builder;
                builder.addUniformBuffer(0, true, false);
                modelBindGroupLayout = builder.build(device);
            }

            createMaterialBindGroupLayout(BuiltinShader::Unlit);
            createMaterialBindGroupLayout(BuiltinShader::UnlitTextured);
            createMaterialBindGroupLayout(BuiltinShader::Sprite);
        }

        void createMaterialBindGroupLayout(BuiltinShader shader) {
            const auto& shaderDef = getShaderDefinition(shader);

            BindGroupLayoutBuilder builder;
            builder.addUniformBuffer(0, false, true);

            if (shaderDef.hasTexture) {
                builder.addTexture(1);
                builder.addSampler(2);
            }

            materialBindGroupLayouts[shader] = builder.build(device);
        }

        void releaseBindGroupLayouts() {
            if (cameraBindGroupLayout) {
                wgpuBindGroupLayoutRelease(cameraBindGroupLayout);
                cameraBindGroupLayout = nullptr;
            }
            if (modelBindGroupLayout) {
                wgpuBindGroupLayoutRelease(modelBindGroupLayout);
                modelBindGroupLayout = nullptr;
            }
            for (auto& [shader, layout] : materialBindGroupLayouts) {
                if (layout) {
                    wgpuBindGroupLayoutRelease(layout);
                }
            }
            materialBindGroupLayouts.clear();
        }
    };

    ShaderRegistry::ShaderRegistry() : m_pImpl(std::make_unique<Impl>()) {}

    ShaderRegistry::~ShaderRegistry() {
        shutdown();
    }

    ShaderRegistry::ShaderRegistry(ShaderRegistry&&) noexcept = default;
    ShaderRegistry& ShaderRegistry::operator=(ShaderRegistry&&) noexcept = default;

    core::Result ShaderRegistry::initialize(GPUDevice* device) {
        m_pImpl->device = device;
        m_pImpl->createBindGroupLayouts();

        spdlog::info("ShaderRegistry initialized");
        return core::Result::Success;
    }

    void ShaderRegistry::shutdown() {
        m_pImpl->pipelines.clear();
        m_pImpl->releaseBindGroupLayouts();
    }

    GPUPipeline* ShaderRegistry::getPipeline(BuiltinShader shader) {
        auto it = m_pImpl->pipelines.find(shader);
        if (it != m_pImpl->pipelines.end()) {
            return it->second.get();
        }

        const auto& shaderDef = getShaderDefinition(shader);

        PipelineConfig config;
        config.vertexShader.code = std::string(shaderDef.vertexCode);
        config.vertexShader.entryPoint = "main";
        config.fragmentShader.code = std::string(shaderDef.fragmentCode);
        config.fragmentShader.entryPoint = "main";
        config.vertexLayout = shaderDef.vertexLayout;
        config.enableBlending = true;

        config.bindGroupLayouts = {
            m_pImpl->cameraBindGroupLayout,
            m_pImpl->modelBindGroupLayout,
            m_pImpl->materialBindGroupLayouts[shader]
        };

        auto pipeline = std::make_unique<GPUPipeline>();
        if (pipeline->initialize(m_pImpl->device, config) != core::Result::Success) {
            spdlog::error("Failed to create pipeline for shader {}", static_cast<int>(shader));
            return nullptr;
        }

        auto* ptr = pipeline.get();
        m_pImpl->pipelines[shader] = std::move(pipeline);

        spdlog::info("Created pipeline for builtin shader {}", static_cast<int>(shader));
        return ptr;
    }

    WGPUBindGroupLayout ShaderRegistry::getCameraBindGroupLayout() const {
        return m_pImpl->cameraBindGroupLayout;
    }

    WGPUBindGroupLayout ShaderRegistry::getModelBindGroupLayout() const {
        return m_pImpl->modelBindGroupLayout;
    }

    WGPUBindGroupLayout ShaderRegistry::getMaterialBindGroupLayout(BuiltinShader shader) const {
        auto it = m_pImpl->materialBindGroupLayouts.find(shader);
        if (it != m_pImpl->materialBindGroupLayouts.end()) {
            return it->second;
        }
        return nullptr;
    }

} // namespace game_engine::graphics
