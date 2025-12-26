#pragma once

#include <game_engine/core/types.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <game_engine/graphics/gpu_pipeline.hpp>
#include <game_engine/graphics/gpu_buffer.hpp>
#include <memory>

namespace game_engine::infrastructure {
    class Window;
}

namespace game_engine::graphics {

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

    private:
        std::unique_ptr<GPUDevice> m_device;
        std::unique_ptr<GPUPipeline> m_pipeline;
        std::unique_ptr<GPUBuffer> m_vertexBuffer;
        infrastructure::Window* m_window = nullptr;
    };

} // namespace game_engine::graphics
