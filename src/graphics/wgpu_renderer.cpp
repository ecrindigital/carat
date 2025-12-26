#include <game_engine/graphics/wgpu_renderer.hpp>
#include <game_engine/infrastructure/window.hpp>
#include <webgpu/webgpu.h>
#include <spdlog/spdlog.h>
#include <array>

namespace game_engine::graphics {

    constexpr const char* VERTEX_SHADER_WGSL = R"(
@vertex
fn main(@location(0) position: vec3<f32>) -> @builtin(position) vec4<f32> {
    return vec4<f32>(position, 1.0);
}
)";

    constexpr const char* FRAGMENT_SHADER_WGSL = R"(
@fragment
fn main() -> @location(0) vec4<f32> {
    return vec4<f32>(1.0, 0.5, 0.2, 1.0);
}
)";

    WGPURenderer::WGPURenderer() = default;

    WGPURenderer::~WGPURenderer() {
        shutdown();
    }

    core::Result WGPURenderer::initialize(infrastructure::Window* window) {
        spdlog::info("Initializing WebGPU Renderer");
        m_window = window;

        m_device = std::make_unique<GPUDevice>();
        if (m_device->initialize(window) != core::Result::Success) {
            spdlog::error("Failed to initialize GPU device");
            return core::Result::Error;
        }

        constexpr std::array<float, 9> vertices = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        m_vertexBuffer = std::make_unique<GPUBuffer>();
        if (m_vertexBuffer->initialize(
                m_device.get(),
                vertices.size() * sizeof(float),
                BufferUsage::Vertex
            ) != core::Result::Success) {
            spdlog::error("Failed to create vertex buffer");
            return core::Result::Error;
        }
        m_vertexBuffer->write(vertices.data(), vertices.size() * sizeof(float));

        PipelineConfig pipelineConfig;
        pipelineConfig.vertexShader.code = VERTEX_SHADER_WGSL;
        pipelineConfig.vertexShader.entryPoint = "main";
        pipelineConfig.fragmentShader.code = FRAGMENT_SHADER_WGSL;
        pipelineConfig.fragmentShader.entryPoint = "main";

        m_pipeline = std::make_unique<GPUPipeline>();
        if (m_pipeline->initialize(m_device.get(), pipelineConfig) != core::Result::Success) {
            spdlog::error("Failed to create render pipeline");
            return core::Result::Error;
        }

        spdlog::info("WebGPU Renderer initialized successfully");
        return core::Result::Success;
    }

    void WGPURenderer::shutdown() {
        m_pipeline.reset();
        m_vertexBuffer.reset();
        m_device.reset();
    }

    void WGPURenderer::beginFrame() {}

    void WGPURenderer::endFrame() {}

    void WGPURenderer::render() {
        if (!m_device || !m_pipeline || !m_vertexBuffer) return;

        WGPUSurface surface = m_device->getSurface();
        WGPUDevice device = m_device->getDevice();
        WGPUQueue queue = m_device->getQueue();

        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);

        if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
            spdlog::warn("Failed to get surface texture");
            return;
        }

        WGPUTextureViewDescriptor viewDesc = {};
        viewDesc.format = WGPUTextureFormat_BGRA8Unorm;
        viewDesc.dimension = WGPUTextureViewDimension_2D;
        viewDesc.baseMipLevel = 0;
        viewDesc.mipLevelCount = 1;
        viewDesc.baseArrayLayer = 0;
        viewDesc.arrayLayerCount = 1;
        viewDesc.aspect = WGPUTextureAspect_All;
        WGPUTextureView textureView = wgpuTextureCreateView(surfaceTexture.texture, &viewDesc);

        WGPUCommandEncoderDescriptor encoderDesc = {};
        WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDesc);

        WGPURenderPassColorAttachment colorAttachment = {};
        colorAttachment.view = textureView;
        colorAttachment.loadOp = WGPULoadOp_Clear;
        colorAttachment.storeOp = WGPUStoreOp_Store;
        colorAttachment.clearValue = {0.2, 0.3, 0.3, 1.0};

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &colorAttachment;

        WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

        wgpuRenderPassEncoderSetPipeline(renderPass, m_pipeline->getPipeline());
        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, m_vertexBuffer->getBuffer(), 0, m_vertexBuffer->getSize());
        wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);

        wgpuRenderPassEncoderEnd(renderPass);

        WGPUCommandBufferDescriptor cmdBufferDesc = {};
        WGPUCommandBuffer cmdBuffer = wgpuCommandEncoderFinish(encoder, &cmdBufferDesc);
        wgpuQueueSubmit(queue, 1, &cmdBuffer);

        m_device->present();

        wgpuCommandBufferRelease(cmdBuffer);
        wgpuRenderPassEncoderRelease(renderPass);
        wgpuCommandEncoderRelease(encoder);
        wgpuTextureViewRelease(textureView);
        wgpuTextureRelease(surfaceTexture.texture);
    }

} // namespace game_engine::graphics
