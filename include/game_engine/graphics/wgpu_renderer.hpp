#pragma once

#include <game_engine/core/types.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <game_engine/graphics/gpu_pipeline.hpp>
#include <game_engine/graphics/gpu_buffer.hpp>
#include <webgpu/webgpu.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace game_engine::infrastructure {
    class Window;
}

namespace game_engine::graphics {

    class ShaderRegistry;
    class Mesh;
    class GPUBindGroup;

    class WGPURenderer {
    public:
        WGPURenderer();
        ~WGPURenderer();

        WGPURenderer(const WGPURenderer&) = delete;
        WGPURenderer& operator=(const WGPURenderer&) = delete;

        [[nodiscard]] core::Result initialize(infrastructure::Window* window);
        void shutdown();

        void beginFrame();
        void endFrame();
        void render();

        void addMesh(Mesh* mesh);
        void removeMesh(Mesh* mesh);
        void clearMeshes();

        void setViewMatrix(const glm::mat4& view);
        void setProjectionMatrix(const glm::mat4& projection);

        [[nodiscard]] GPUDevice* getDevice() const;
        [[nodiscard]] ShaderRegistry* getShaderRegistry() const;

    private:
        std::unique_ptr<GPUDevice> m_device;
        std::unique_ptr<ShaderRegistry> m_shaderRegistry;

        std::unique_ptr<GPUBuffer> m_cameraUniformBuffer;
        std::unique_ptr<GPUBindGroup> m_cameraBindGroup;
        glm::mat4 m_viewMatrix{1.0f};
        glm::mat4 m_projectionMatrix{1.0f};
        bool m_cameraDirty = true;

        std::vector<Mesh*> m_meshes;

        std::unique_ptr<GPUPipeline> m_legacyPipeline;
        std::unique_ptr<GPUBuffer> m_legacyVertexBuffer;
        bool m_useLegacyMode = true;

        infrastructure::Window* m_window = nullptr;

        WGPUTexture m_depthTexture = nullptr;
        WGPUTextureView m_depthTextureView = nullptr;
        void createDepthTexture(uint32_t width, uint32_t height);

        void renderLegacy();
        void renderMeshes();
        void updateCameraUniforms();
    };

} // namespace game_engine::graphics
