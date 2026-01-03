#include <game_engine/graphics/wgpu_renderer.hpp>
#include <game_engine/graphics/shader_registry.hpp>
#include <game_engine/graphics/mesh.hpp>
#include <game_engine/graphics/material.hpp>
#include <game_engine/graphics/gpu_bind_group.hpp>
#include <game_engine/infrastructure/window.hpp>
#include <webgpu/webgpu.h>
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <algorithm>

namespace game_engine::graphics {

    struct alignas(16) CameraData {
        glm::mat4 view{1.0f};
        glm::mat4 projection{1.0f};
    };

    constexpr const char* LEGACY_VERTEX_SHADER = R"(
@vertex
fn main(@location(0) position: vec3<f32>) -> @builtin(position) vec4<f32> {
    return vec4<f32>(position, 1.0);
}
)";

    constexpr const char* LEGACY_FRAGMENT_SHADER = R"(
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

        m_shaderRegistry = std::make_unique<ShaderRegistry>();
        if (m_shaderRegistry->initialize(m_device.get()) != core::Result::Success) {
            spdlog::error("Failed to initialize shader registry");
            return core::Result::Error;
        }

        m_cameraUniformBuffer = std::make_unique<GPUBuffer>();
        if (m_cameraUniformBuffer->initialize(
                m_device.get(),
                sizeof(CameraData),
                BufferUsage::Uniform
            ) != core::Result::Success) {
            spdlog::error("Failed to create camera uniform buffer");
            return core::Result::Error;
        }

        std::vector<BindGroupEntry> cameraEntries = {{
            0, BindingType::UniformBuffer, m_cameraUniformBuffer.get(), 0, sizeof(CameraData), nullptr
        }};

        m_cameraBindGroup = std::make_unique<GPUBindGroup>();
        if (m_cameraBindGroup->initialize(
                m_device.get(),
                m_shaderRegistry->getCameraBindGroupLayout(),
                cameraEntries
            ) != core::Result::Success) {
            spdlog::error("Failed to create camera bind group");
            return core::Result::Error;
        }

        auto windowSize = window->getSize();
        float aspect = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
        m_projectionMatrix = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
        m_viewMatrix = glm::mat4(1.0f);
        updateCameraUniforms();

        createDepthTexture(windowSize.x, windowSize.y);

        constexpr std::array<float, 9> legacyVertices = {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.0f,  0.5f, 0.0f
        };

        m_legacyVertexBuffer = std::make_unique<GPUBuffer>();
        if (m_legacyVertexBuffer->initialize(
                m_device.get(),
                legacyVertices.size() * sizeof(float),
                BufferUsage::Vertex
            ) != core::Result::Success) {
            spdlog::error("Failed to create legacy vertex buffer");
            return core::Result::Error;
        }
        m_legacyVertexBuffer->write(legacyVertices.data(), legacyVertices.size() * sizeof(float));

        PipelineConfig legacyConfig;
        legacyConfig.vertexShader.code = LEGACY_VERTEX_SHADER;
        legacyConfig.vertexShader.entryPoint = "main";
        legacyConfig.fragmentShader.code = LEGACY_FRAGMENT_SHADER;
        legacyConfig.fragmentShader.entryPoint = "main";

        m_legacyPipeline = std::make_unique<GPUPipeline>();
        if (m_legacyPipeline->initialize(m_device.get(), legacyConfig) != core::Result::Success) {
            spdlog::error("Failed to create legacy render pipeline");
            return core::Result::Error;
        }

        spdlog::info("WebGPU Renderer initialized successfully");
        return core::Result::Success;
    }

    void WGPURenderer::createDepthTexture(uint32_t width, uint32_t height) {
        if (m_depthTextureView) {
            wgpuTextureViewRelease(m_depthTextureView);
            m_depthTextureView = nullptr;
        }
        if (m_depthTexture) {
            wgpuTextureDestroy(m_depthTexture);
            wgpuTextureRelease(m_depthTexture);
            m_depthTexture = nullptr;
        }

        WGPUTextureDescriptor depthTextureDesc = {};
        depthTextureDesc.label = "Depth Texture";
        depthTextureDesc.size.width = width;
        depthTextureDesc.size.height = height;
        depthTextureDesc.size.depthOrArrayLayers = 1;
        depthTextureDesc.mipLevelCount = 1;
        depthTextureDesc.sampleCount = 1;
        depthTextureDesc.dimension = WGPUTextureDimension_2D;
        depthTextureDesc.format = WGPUTextureFormat_Depth24Plus;
        depthTextureDesc.usage = WGPUTextureUsage_RenderAttachment;

        m_depthTexture = wgpuDeviceCreateTexture(m_device->getDevice(), &depthTextureDesc);
        if (!m_depthTexture) {
            spdlog::error("Failed to create depth texture");
            return;
        }

        WGPUTextureViewDescriptor depthViewDesc = {};
        depthViewDesc.format = WGPUTextureFormat_Depth24Plus;
        depthViewDesc.dimension = WGPUTextureViewDimension_2D;
        depthViewDesc.baseMipLevel = 0;
        depthViewDesc.mipLevelCount = 1;
        depthViewDesc.baseArrayLayer = 0;
        depthViewDesc.arrayLayerCount = 1;
        depthViewDesc.aspect = WGPUTextureAspect_DepthOnly;

        m_depthTextureView = wgpuTextureCreateView(m_depthTexture, &depthViewDesc);
        if (!m_depthTextureView) {
            spdlog::error("Failed to create depth texture view");
            return;
        }

        spdlog::info("Depth texture created: {}x{}", width, height);
    }

    void WGPURenderer::shutdown() {
        m_meshes.clear();
        m_cameraBindGroup.reset();
        m_cameraUniformBuffer.reset();
        m_legacyPipeline.reset();
        m_legacyVertexBuffer.reset();
        m_shaderRegistry.reset();

        if (m_depthTextureView) {
            wgpuTextureViewRelease(m_depthTextureView);
            m_depthTextureView = nullptr;
        }
        if (m_depthTexture) {
            wgpuTextureDestroy(m_depthTexture);
            wgpuTextureRelease(m_depthTexture);
            m_depthTexture = nullptr;
        }

        m_device.reset();
    }

    void WGPURenderer::beginFrame() {}

    void WGPURenderer::endFrame() {}

    void WGPURenderer::render() {
        if (m_useLegacyMode && m_meshes.empty()) {
            renderLegacy();
        } else {
            renderMeshes();
        }
    }

    void WGPURenderer::renderLegacy() {
        if (!m_device || !m_legacyPipeline || !m_legacyVertexBuffer) return;

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
        colorAttachment.clearValue = {0.0, 0.0, 0.0, 1.0};

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &colorAttachment;

        WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

        wgpuRenderPassEncoderSetPipeline(renderPass, m_legacyPipeline->getPipeline());
        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, m_legacyVertexBuffer->getBuffer(), 0, m_legacyVertexBuffer->getSize());
        wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);

        wgpuRenderPassEncoderEnd(renderPass);

        WGPUCommandBufferDescriptor cmdBufferDesc = {};
        WGPUCommandBuffer cmdBuffer = wgpuCommandEncoderFinish(encoder, &cmdBufferDesc);
        wgpuQueueSubmit(queue, 1, &cmdBuffer);
        m_device->poll(false);

        m_device->present();

        wgpuCommandBufferRelease(cmdBuffer);
        wgpuRenderPassEncoderRelease(renderPass);
        wgpuCommandEncoderRelease(encoder);
        wgpuTextureViewRelease(textureView);
    }

    void WGPURenderer::renderMeshes() {
        if (!m_device || m_meshes.empty()) return;

        if (m_cameraDirty) {
            updateCameraUniforms();
        }

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
        colorAttachment.clearValue = {0.0, 0.0, 0.0, 1.0};

        WGPURenderPassDepthStencilAttachment depthStencilAttachment = {};
        depthStencilAttachment.view = m_depthTextureView;
        depthStencilAttachment.depthLoadOp = WGPULoadOp_Clear;
        depthStencilAttachment.depthStoreOp = WGPUStoreOp_Store;
        depthStencilAttachment.depthClearValue = 1.0f;
        depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
        depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Discard;
        depthStencilAttachment.stencilClearValue = 0;

        WGPURenderPassDescriptor renderPassDesc = {};
        renderPassDesc.colorAttachmentCount = 1;
        renderPassDesc.colorAttachments = &colorAttachment;
        renderPassDesc.depthStencilAttachment = m_depthTextureView ? &depthStencilAttachment : nullptr;

        WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDesc);

        for (Mesh* mesh : m_meshes) {
            if (!mesh) continue;

            Material* material = mesh->getMaterial();
            if (!material) continue;

            GPUPipeline* pipeline = m_shaderRegistry->getPipeline(material->getShader());
            if (!pipeline) continue;

            mesh->updateModelUniformBuffer();
            material->updateUniformBuffer();

            wgpuRenderPassEncoderSetPipeline(renderPass, pipeline->getPipeline());
            wgpuRenderPassEncoderSetBindGroup(renderPass, 0, m_cameraBindGroup->getBindGroup(), 0, nullptr);

            if (mesh->getModelBindGroup()) {
                wgpuRenderPassEncoderSetBindGroup(renderPass, 1, mesh->getModelBindGroup()->getBindGroup(), 0, nullptr);
            }

            if (material->getBindGroup()) {
                wgpuRenderPassEncoderSetBindGroup(renderPass, 2, material->getBindGroup()->getBindGroup(), 0, nullptr);
            }

            GPUBuffer* vertexBuffer = mesh->getVertexBuffer();
            if (vertexBuffer) {
                wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vertexBuffer->getBuffer(), 0, vertexBuffer->getSize());
            }

            if (mesh->hasIndices()) {
                GPUBuffer* indexBuffer = mesh->getIndexBuffer();
                WGPUIndexFormat format = mesh->isIndexFormat32() ? WGPUIndexFormat_Uint32 : WGPUIndexFormat_Uint16;
                wgpuRenderPassEncoderSetIndexBuffer(renderPass, indexBuffer->getBuffer(), format, 0, indexBuffer->getSize());
                wgpuRenderPassEncoderDrawIndexed(renderPass, mesh->getIndexCount(), 1, 0, 0, 0);
            } else {
                wgpuRenderPassEncoderDraw(renderPass, mesh->getVertexCount(), 1, 0, 0);
            }
        }

        wgpuRenderPassEncoderEnd(renderPass);

        WGPUCommandBufferDescriptor cmdBufferDesc = {};
        WGPUCommandBuffer cmdBuffer = wgpuCommandEncoderFinish(encoder, &cmdBufferDesc);
        wgpuQueueSubmit(queue, 1, &cmdBuffer);
        m_device->poll(false);

        m_device->present();

        wgpuCommandBufferRelease(cmdBuffer);
        wgpuRenderPassEncoderRelease(renderPass);
        wgpuCommandEncoderRelease(encoder);
        wgpuTextureViewRelease(textureView);
    }

    void WGPURenderer::updateCameraUniforms() {
        CameraData cameraData;
        cameraData.view = m_viewMatrix;
        cameraData.projection = m_projectionMatrix;
        m_cameraUniformBuffer->write(&cameraData, sizeof(CameraData));
        m_cameraDirty = false;
    }

    void WGPURenderer::addMesh(Mesh* mesh) {
        if (mesh) {
            m_meshes.push_back(mesh);
            m_useLegacyMode = false;
        }
    }

    void WGPURenderer::removeMesh(Mesh* mesh) {
        auto it = std::find(m_meshes.begin(), m_meshes.end(), mesh);
        if (it != m_meshes.end()) {
            m_meshes.erase(it);
        }
        if (m_meshes.empty()) {
            m_useLegacyMode = true;
        }
    }

    void WGPURenderer::clearMeshes() {
        m_meshes.clear();
        m_useLegacyMode = true;
    }

    void WGPURenderer::setViewMatrix(const glm::mat4& view) {
        m_viewMatrix = view;
        m_cameraDirty = true;
    }

    void WGPURenderer::setProjectionMatrix(const glm::mat4& projection) {
        m_projectionMatrix = projection;
        m_cameraDirty = true;
    }

    GPUDevice* WGPURenderer::getDevice() const {
        return m_device.get();
    }

    ShaderRegistry* WGPURenderer::getShaderRegistry() const {
        return m_shaderRegistry.get();
    }

} // namespace game_engine::graphics
