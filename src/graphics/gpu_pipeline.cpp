#include <game_engine/graphics/gpu_pipeline.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <webgpu/webgpu.h>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    class GPUPipeline::Impl {
    public:
        WGPURenderPipeline pipeline = nullptr;
        WGPUShaderModule vertexShader = nullptr;
        WGPUShaderModule fragmentShader = nullptr;
        WGPUDevice device = nullptr;
    };

    GPUPipeline::GPUPipeline() : m_pImpl(std::make_unique<Impl>()) {}

    GPUPipeline::~GPUPipeline() {
        shutdown();
    }

    GPUPipeline::GPUPipeline(GPUPipeline&&) noexcept = default;
    GPUPipeline& GPUPipeline::operator=(GPUPipeline&&) noexcept = default;

    core::Result GPUPipeline::initialize(GPUDevice* device, const PipelineConfig& config) {
        m_pImpl->device = device->getDevice();

        WGPUShaderModuleWGSLDescriptor wgslDesc = {};
        wgslDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        wgslDesc.code = config.vertexShader.code.c_str();

        WGPUShaderModuleDescriptor shaderDesc = {};
        shaderDesc.nextInChain = reinterpret_cast<const WGPUChainedStruct*>(&wgslDesc);

        m_pImpl->vertexShader = wgpuDeviceCreateShaderModule(m_pImpl->device, &shaderDesc);
        if (!m_pImpl->vertexShader) {
            spdlog::error("Failed to create vertex shader module");
            return core::Result::Error;
        }

        wgslDesc.code = config.fragmentShader.code.c_str();
        m_pImpl->fragmentShader = wgpuDeviceCreateShaderModule(m_pImpl->device, &shaderDesc);
        if (!m_pImpl->fragmentShader) {
            spdlog::error("Failed to create fragment shader module");
            return core::Result::Error;
        }

        WGPUVertexAttribute vertexAttrib = {};
        vertexAttrib.format = WGPUVertexFormat_Float32x3;
        vertexAttrib.offset = 0;
        vertexAttrib.shaderLocation = 0;

        WGPUVertexBufferLayout vertexBufferLayout = {};
        vertexBufferLayout.arrayStride = 3 * sizeof(float);
        vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;
        vertexBufferLayout.attributeCount = 1;
        vertexBufferLayout.attributes = &vertexAttrib;

        WGPUBlendState blendState = {};
        blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
        blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.color.operation = WGPUBlendOperation_Add;
        blendState.alpha.srcFactor = WGPUBlendFactor_One;
        blendState.alpha.dstFactor = WGPUBlendFactor_Zero;
        blendState.alpha.operation = WGPUBlendOperation_Add;

        WGPUColorTargetState colorTarget = {};
        colorTarget.format = WGPUTextureFormat_BGRA8Unorm;
        colorTarget.blend = &blendState;
        colorTarget.writeMask = WGPUColorWriteMask_All;

        WGPUFragmentState fragmentState = {};
        fragmentState.module = m_pImpl->fragmentShader;
        fragmentState.entryPoint = config.fragmentShader.entryPoint.c_str();
        fragmentState.targetCount = 1;
        fragmentState.targets = &colorTarget;

        WGPURenderPipelineDescriptor pipelineDesc = {};
        pipelineDesc.label = "Triangle Pipeline";

        pipelineDesc.vertex.module = m_pImpl->vertexShader;
        pipelineDesc.vertex.entryPoint = config.vertexShader.entryPoint.c_str();
        pipelineDesc.vertex.bufferCount = 1;
        pipelineDesc.vertex.buffers = &vertexBufferLayout;

        pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
        pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
        pipelineDesc.primitive.cullMode = WGPUCullMode_None;

        pipelineDesc.multisample.count = 1;
        pipelineDesc.multisample.mask = ~0u;
        pipelineDesc.multisample.alphaToCoverageEnabled = false;

        pipelineDesc.fragment = &fragmentState;

        m_pImpl->pipeline = wgpuDeviceCreateRenderPipeline(m_pImpl->device, &pipelineDesc);
        if (!m_pImpl->pipeline) {
            spdlog::error("Failed to create render pipeline");
            return core::Result::Error;
        }

        spdlog::info("WebGPU render pipeline created successfully");
        return core::Result::Success;
    }

    void GPUPipeline::shutdown() {
        if (m_pImpl->pipeline) {
            wgpuRenderPipelineRelease(m_pImpl->pipeline);
            m_pImpl->pipeline = nullptr;
        }
        if (m_pImpl->vertexShader) {
            wgpuShaderModuleRelease(m_pImpl->vertexShader);
            m_pImpl->vertexShader = nullptr;
        }
        if (m_pImpl->fragmentShader) {
            wgpuShaderModuleRelease(m_pImpl->fragmentShader);
            m_pImpl->fragmentShader = nullptr;
        }
    }

    WGPURenderPipeline GPUPipeline::getPipeline() const { return m_pImpl->pipeline; }

} // namespace game_engine::graphics
