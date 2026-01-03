#pragma once

#include <game_engine/core/types.hpp>
#include <memory>
#include <cstdint>
#include <span>
#include <array>

typedef struct WGPUTextureImpl* WGPUTexture;
typedef struct WGPUTextureViewImpl* WGPUTextureView;
typedef struct WGPUSamplerImpl* WGPUSampler;

namespace game_engine::graphics {

    class GPUDevice;

    struct TextureConfig {
        uint32_t width = 1;
        uint32_t height = 1;
        bool generateMipmaps = false;
    };

    class GPUTexture {
    public:
        GPUTexture();
        ~GPUTexture();

        GPUTexture(const GPUTexture&) = delete;
        GPUTexture& operator=(const GPUTexture&) = delete;
        GPUTexture(GPUTexture&&) noexcept;
        GPUTexture& operator=(GPUTexture&&) noexcept;

        [[nodiscard]] core::Result initialize(GPUDevice* device, const TextureConfig& config);
        [[nodiscard]] core::Result initializeFromData(GPUDevice* device, const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels = 4);
        [[nodiscard]] core::Result initializeFromHDRData(GPUDevice* device, const float* data, uint32_t width, uint32_t height);
        [[nodiscard]] core::Result initializeCubemap(GPUDevice* device, const std::array<unsigned char*, 6>& faceData, uint32_t size);
        [[nodiscard]] core::Result initializeDefault(GPUDevice* device);
        void shutdown();

        [[nodiscard]] bool isCubemap() const;

        [[nodiscard]] WGPUTexture getTexture() const;
        [[nodiscard]] WGPUTextureView getTextureView() const;
        [[nodiscard]] WGPUSampler getSampler() const;
        [[nodiscard]] uint32_t getWidth() const;
        [[nodiscard]] uint32_t getHeight() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

}
