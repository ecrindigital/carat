#include <game_engine/graphics/gpu_pipeline.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <webgpu/webgpu.h>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    namespace {
        WGPUVertexFormat toWGPUFormat(VertexFormat format) {
            switch (format) {
                case VertexFormat::Float32x2: return WGPUVertexFormat_Float32x2;
                case VertexFormat::Float32x3: return WGPUVertexFormat_Float32x3;
                case VertexFormat::Float32x4: return WGPUVertexFormat_Float32x4;
                default: return WGPUVertexFormat_Float32x3;
            }
        }
    }

    class GPUPipeline::Impl {
    public:
        WGPURenderPipeline pipeline = nullptr;
        WGPUPipelineLayout pipelineLayout = nullptr;
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

        std::vector<WGPUVertexAttribute> vertexAttribs;
        uint32_t stride = 3 * sizeof(float);

        if (config.vertexLayout.has_value()) {
            VertexLayout layout = getVertexLayout(config.vertexLayout.value());
            stride = layout.stride;

            for (const auto& attr : layout.attributes) {
                WGPUVertexAttribute wgpuAttr = {};
                wgpuAttr.format = toWGPUFormat(attr.format);
                wgpuAttr.offset = attr.offset;
                wgpuAttr.shaderLocation = attr.location;
                vertexAttribs.push_back(wgpuAttr);
            }
        } else {
            WGPUVertexAttribute posAttr = {};
            posAttr.format = WGPUVertexFormat_Float32x3;
            posAttr.offset = 0;
            posAttr.shaderLocation = 0;
            vertexAttribs.push_back(posAttr);
        }

        WGPUVertexBufferLayout vertexBufferLayout = {};
        vertexBufferLayout.arrayStride = stride;
        vertexBufferLayout.stepMode = WGPUVertexStepMode_Vertex;
        vertexBufferLayout.attributeCount = static_cast<uint32_t>(vertexAttribs.size());
        vertexBufferLayout.attributes = vertexAttribs.data();

        if (!config.bindGroupLayouts.empty()) {
            WGPUPipelineLayoutDescriptor layoutDesc = {};
            layoutDesc.bindGroupLayoutCount = static_cast<uint32_t>(config.bindGroupLayouts.size());
            layoutDesc.bindGroupLayouts = config.bindGroupLayouts.data();

            m_pImpl->pipelineLayout = wgpuDeviceCreatePipelineLayout(m_pImpl->device, &layoutDesc);
            if (!m_pImpl->pipelineLayout) {
                spdlog::error("Failed to create pipeline layout");
                return core::Result::Error;
            }
        }

        WGPUBlendState blendState = {};
        if (config.enableBlending) {
            blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
            blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
            blendState.color.operation = WGPUBlendOperation_Add;
            blendState.alpha.srcFactor = WGPUBlendFactor_One;
            blendState.alpha.dstFactor = WGPUBlendFactor_Zero;
            blendState.alpha.operation = WGPUBlendOperation_Add;
        }

        WGPUColorTargetState colorTarget = {};
        colorTarget.format = WGPUTextureFormat_BGRA8Unorm;
        colorTarget.blend = config.enableBlending ? &blendState : nullptr;
        colorTarget.writeMask = WGPUColorWriteMask_All;

        WGPUFragmentState fragmentState = {};
        fragmentState.module = m_pImpl->fragmentShader;
        fragmentState.entryPoint = config.fragmentShader.entryPoint.c_str();
        fragmentState.targetCount = 1;
        fragmentState.targets = &colorTarget;

        WGPUDepthStencilState depthStencilState = {};
        if (config.enableDepthTest) {
            depthStencilState.format = WGPUTextureFormat_Depth24Plus;
            depthStencilState.depthWriteEnabled = true;
            depthStencilState.depthCompare = WGPUCompareFunction_Less;
        }

        WGPURenderPipelineDescriptor pipelineDesc = {};
        pipelineDesc.label = "Render Pipeline";
        pipelineDesc.layout = m_pImpl->pipelineLayout;

        pipelineDesc.vertex.module = m_pImpl->vertexShader;
        pipelineDesc.vertex.entryPoint = config.vertexShader.entryPoint.c_str();
        pipelineDesc.vertex.bufferCount = 1;
        pipelineDesc.vertex.buffers = &vertexBufferLayout;

        pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
        pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
        pipelineDesc.primitive.cullMode = WGPUCullMode_None;

        pipelineDesc.depthStencil = config.enableDepthTest ? &depthStencilState : nullptr;

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
        if (m_pImpl->pipelineLayout) {
            wgpuPipelineLayoutRelease(m_pImpl->pipelineLayout);
            m_pImpl->pipelineLayout = nullptr;
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
    WGPUPipelineLayout GPUPipeline::getPipelineLayout() const { return m_pImpl->pipelineLayout; }

}
