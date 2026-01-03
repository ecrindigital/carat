#pragma once

#include <game_engine/core/types.hpp>
#include <memory>
#include <string>

typedef struct WGPUInstanceImpl* WGPUInstance;
typedef struct WGPUAdapterImpl* WGPUAdapter;
typedef struct WGPUDeviceImpl* WGPUDevice;
typedef struct WGPUQueueImpl* WGPUQueue;
typedef struct WGPUSurfaceImpl* WGPUSurface;

namespace game_engine::infrastructure {
    class Window;
}

namespace game_engine::graphics {

    struct GPUDeviceConfig {
        bool enableValidation = true;
        std::string adapterName;
    };

    class GPUDevice {
    public:
        GPUDevice();
        ~GPUDevice();

        GPUDevice(const GPUDevice&) = delete;
        GPUDevice& operator=(const GPUDevice&) = delete;
        GPUDevice(GPUDevice&&) noexcept;
        GPUDevice& operator=(GPUDevice&&) noexcept;

        [[nodiscard]] core::Result initialize(infrastructure::Window* window, const GPUDeviceConfig& config = {});
        void shutdown();

        [[nodiscard]] WGPUDevice getDevice() const;
        [[nodiscard]] WGPUQueue getQueue() const;
        [[nodiscard]] WGPUSurface getSurface() const;
        [[nodiscard]] WGPUInstance getInstance() const;

        void present();
        void poll(bool wait = false);

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

} // namespace game_engine::graphics
