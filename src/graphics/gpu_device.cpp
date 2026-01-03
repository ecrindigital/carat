#include <game_engine/graphics/gpu_device.hpp>
#include <game_engine/infrastructure/window.hpp>
#include <webgpu/webgpu.h>
#include <webgpu/wgpu.h> 
#include <spdlog/spdlog.h>
#include <SDL3/SDL.h>

#ifdef __APPLE__
#include <SDL3/SDL_metal.h>
#endif

namespace game_engine::graphics {

    class GPUDevice::Impl {
    public:
        WGPUInstance instance = nullptr;
        WGPUAdapter adapter = nullptr;
        WGPUDevice device = nullptr;
        WGPUQueue queue = nullptr;
        WGPUSurface surface = nullptr;
        infrastructure::Window* window = nullptr;

        static void onDeviceError(WGPUErrorType type, const char* message, void* userdata) {
            spdlog::error("WebGPU Device Error ({}): {}", static_cast<int>(type), message);
        }

        static void onDeviceLost(WGPUDeviceLostReason reason, const char* message, void* userdata) {
            spdlog::error("WebGPU Device Lost ({}): {}", static_cast<int>(reason), message);
        }
    };

    GPUDevice::GPUDevice() : m_pImpl(std::make_unique<Impl>()) {}

    GPUDevice::~GPUDevice() {
        shutdown();
    }

    GPUDevice::GPUDevice(GPUDevice&&) noexcept = default;
    GPUDevice& GPUDevice::operator=(GPUDevice&&) noexcept = default;

    core::Result GPUDevice::initialize(infrastructure::Window* window, const GPUDeviceConfig& config) {
        spdlog::info("Initializing WebGPU GPUDevice");
        m_pImpl->window = window;

        WGPUInstanceDescriptor instanceDesc = {};
        m_pImpl->instance = wgpuCreateInstance(&instanceDesc);
        if (!m_pImpl->instance) {
            spdlog::error("Failed to create WebGPU instance");
            return core::Result::Error;
        }

        auto* sdlWindow = static_cast<SDL_Window*>(window->getNativeHandle());

#ifdef __APPLE__
        SDL_MetalView metalView = SDL_Metal_CreateView(sdlWindow);
        if (!metalView) {
            spdlog::error("Failed to create Metal view: {}", SDL_GetError());
            return core::Result::Error;
        }

        WGPUSurfaceDescriptorFromMetalLayer metalSurfaceDesc = {};
        metalSurfaceDesc.chain.sType = WGPUSType_SurfaceDescriptorFromMetalLayer;
        metalSurfaceDesc.layer = SDL_Metal_GetLayer(metalView);

        WGPUSurfaceDescriptor surfaceDesc = {};
        surfaceDesc.nextInChain = reinterpret_cast<const WGPUChainedStruct*>(&metalSurfaceDesc);

        m_pImpl->surface = wgpuInstanceCreateSurface(m_pImpl->instance, &surfaceDesc);
#else
        spdlog::error("Non-macOS platforms not yet implemented");
        return core::Result::Error;
#endif

        if (!m_pImpl->surface) {
            spdlog::error("Failed to create WebGPU surface");
            return core::Result::Error;
        }

        WGPURequestAdapterOptions adapterOptions = {};
        adapterOptions.compatibleSurface = m_pImpl->surface;
        adapterOptions.powerPreference = WGPUPowerPreference_HighPerformance;

        struct AdapterCallbackData {
            WGPUAdapter adapter = nullptr;
            bool done = false;
        } adapterData;

        wgpuInstanceRequestAdapter(
            m_pImpl->instance,
            &adapterOptions,
            [](WGPURequestAdapterStatus status, WGPUAdapter adapter, const char* message, void* userdata) {
                auto* data = static_cast<AdapterCallbackData*>(userdata);
                if (status == WGPURequestAdapterStatus_Success) {
                    data->adapter = adapter;
                } else {
                    spdlog::error("Failed to request adapter: {}", message ? message : "unknown");
                }
                data->done = true;
            },
            &adapterData
        );

        while (!adapterData.done) {}

        m_pImpl->adapter = adapterData.adapter;
        if (!m_pImpl->adapter) {
            spdlog::error("Failed to get WebGPU adapter");
            return core::Result::Error;
        }

        WGPUDeviceDescriptor deviceDesc = {};
        deviceDesc.label = "Axolotl Device";

        struct DeviceCallbackData {
            WGPUDevice device = nullptr;
            bool done = false;
        } deviceData;

        wgpuAdapterRequestDevice(
            m_pImpl->adapter,
            &deviceDesc,
            [](WGPURequestDeviceStatus status, WGPUDevice device, const char* message, void* userdata) {
                auto* data = static_cast<DeviceCallbackData*>(userdata);
                if (status == WGPURequestDeviceStatus_Success) {
                    data->device = device;
                } else {
                    spdlog::error("Failed to request device: {}", message ? message : "unknown");
                }
                data->done = true;
            },
            &deviceData
        );

        while (!deviceData.done) {}

        m_pImpl->device = deviceData.device;
        if (!m_pImpl->device) {
            spdlog::error("Failed to get WebGPU device");
            return core::Result::Error;
        }

        wgpuDeviceSetUncapturedErrorCallback(m_pImpl->device, Impl::onDeviceError, nullptr);

        m_pImpl->queue = wgpuDeviceGetQueue(m_pImpl->device);

        auto windowSize = window->getSize();
        WGPUSurfaceConfiguration surfaceConfig = {};
        surfaceConfig.device = m_pImpl->device;
        surfaceConfig.format = WGPUTextureFormat_BGRA8Unorm;
        surfaceConfig.usage = WGPUTextureUsage_RenderAttachment;
        surfaceConfig.width = static_cast<uint32_t>(windowSize.x);
        surfaceConfig.height = static_cast<uint32_t>(windowSize.y);
        surfaceConfig.presentMode = WGPUPresentMode_Fifo;
        surfaceConfig.alphaMode = WGPUCompositeAlphaMode_Auto;

        wgpuSurfaceConfigure(m_pImpl->surface, &surfaceConfig);

        spdlog::info("WebGPU GPUDevice initialized successfully");
        return core::Result::Success;
    }

    void GPUDevice::shutdown() {
        if (m_pImpl->queue) {
            wgpuQueueRelease(m_pImpl->queue);
            m_pImpl->queue = nullptr;
        }
        if (m_pImpl->device) {
            wgpuDeviceRelease(m_pImpl->device);
            m_pImpl->device = nullptr;
        }
        if (m_pImpl->adapter) {
            wgpuAdapterRelease(m_pImpl->adapter);
            m_pImpl->adapter = nullptr;
        }
        if (m_pImpl->surface) {
            wgpuSurfaceRelease(m_pImpl->surface);
            m_pImpl->surface = nullptr;
        }
        if (m_pImpl->instance) {
            wgpuInstanceRelease(m_pImpl->instance);
            m_pImpl->instance = nullptr;
        }
    }

    WGPUDevice GPUDevice::getDevice() const { return m_pImpl->device; }
    WGPUQueue GPUDevice::getQueue() const { return m_pImpl->queue; }
    WGPUSurface GPUDevice::getSurface() const { return m_pImpl->surface; }
    WGPUInstance GPUDevice::getInstance() const { return m_pImpl->instance; }

    void GPUDevice::present() {
        wgpuSurfacePresent(m_pImpl->surface);
    }

    void GPUDevice::poll(bool wait) {
        if (m_pImpl->device) {
            wgpuDevicePoll(m_pImpl->device, wait, nullptr);
        }
    }

} // namespace game_engine::graphics
