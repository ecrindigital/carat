#include <game_engine/graphics/gpu_texture.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <webgpu/webgpu.h>
#include <spdlog/spdlog.h>
#include <array>

namespace game_engine::graphics {

    class GPUTexture::Impl {
    public:
        WGPUTexture texture = nullptr;
        WGPUTextureView textureView = nullptr;
        WGPUSampler sampler = nullptr;
        WGPUDevice device = nullptr;
        WGPUQueue queue = nullptr;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    GPUTexture::GPUTexture() : m_pImpl(std::make_unique<Impl>()) {}

    GPUTexture::~GPUTexture() {
        shutdown();
    }

    GPUTexture::GPUTexture(GPUTexture&&) noexcept = default;
    GPUTexture& GPUTexture::operator=(GPUTexture&&) noexcept = default;

    core::Result GPUTexture::initialize(GPUDevice* device, const TextureConfig& config) {
        m_pImpl->device = device->getDevice();
        m_pImpl->queue = device->getQueue();
        m_pImpl->width = config.width;
        m_pImpl->height = config.height;

        WGPUTextureDescriptor textureDesc = {};
        textureDesc.label = "Texture";
        textureDesc.size.width = config.width;
        textureDesc.size.height = config.height;
        textureDesc.size.depthOrArrayLayers = 1;
        textureDesc.mipLevelCount = 1;
        textureDesc.sampleCount = 1;
        textureDesc.dimension = WGPUTextureDimension_2D;
        textureDesc.format = WGPUTextureFormat_RGBA8Unorm;
        textureDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;

        m_pImpl->texture = wgpuDeviceCreateTexture(m_pImpl->device, &textureDesc);
        if (!m_pImpl->texture) {
            spdlog::error("Failed to create texture");
            return core::Result::Error;
        }

        WGPUTextureViewDescriptor viewDesc = {};
        viewDesc.format = WGPUTextureFormat_RGBA8Unorm;
        viewDesc.dimension = WGPUTextureViewDimension_2D;
        viewDesc.baseMipLevel = 0;
        viewDesc.mipLevelCount = 1;
        viewDesc.baseArrayLayer = 0;
        viewDesc.arrayLayerCount = 1;
        viewDesc.aspect = WGPUTextureAspect_All;

        m_pImpl->textureView = wgpuTextureCreateView(m_pImpl->texture, &viewDesc);
        if (!m_pImpl->textureView) {
            spdlog::error("Failed to create texture view");
            return core::Result::Error;
        }

        WGPUSamplerDescriptor samplerDesc = {};
        samplerDesc.addressModeU = WGPUAddressMode_Repeat;
        samplerDesc.addressModeV = WGPUAddressMode_Repeat;
        samplerDesc.addressModeW = WGPUAddressMode_Repeat;
        samplerDesc.magFilter = WGPUFilterMode_Linear;
        samplerDesc.minFilter = WGPUFilterMode_Linear;
        samplerDesc.mipmapFilter = WGPUMipmapFilterMode_Linear;
        samplerDesc.lodMinClamp = 0.0f;
        samplerDesc.lodMaxClamp = 1.0f;
        samplerDesc.maxAnisotropy = 1;

        m_pImpl->sampler = wgpuDeviceCreateSampler(m_pImpl->device, &samplerDesc);
        if (!m_pImpl->sampler) {
            spdlog::error("Failed to create sampler");
            return core::Result::Error;
        }

        return core::Result::Success;
    }

    core::Result GPUTexture::initializeFromData(GPUDevice* device, const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels) {
        TextureConfig config;
        config.width = width;
        config.height = height;

        if (initialize(device, config) != core::Result::Success) {
            return core::Result::Error;
        }

        WGPUImageCopyTexture destination = {};
        destination.texture = m_pImpl->texture;
        destination.mipLevel = 0;
        destination.origin = {0, 0, 0};
        destination.aspect = WGPUTextureAspect_All;

        WGPUTextureDataLayout source = {};
        source.offset = 0;
        source.bytesPerRow = width * channels;
        source.rowsPerImage = height;

        WGPUExtent3D writeSize = {};
        writeSize.width = width;
        writeSize.height = height;
        writeSize.depthOrArrayLayers = 1;

        wgpuQueueWriteTexture(m_pImpl->queue, &destination, data, width * height * channels, &source, &writeSize);

        spdlog::info("Texture created ({}x{}, {} channels)", width, height, channels);
        return core::Result::Success;
    }

    core::Result GPUTexture::initializeDefault(GPUDevice* device) {
        std::array<uint8_t, 4> whitePixel = {255, 255, 255, 255};
        return initializeFromData(device, whitePixel.data(), 1, 1, 4);
    }

    void GPUTexture::shutdown() {
        if (m_pImpl->sampler) {
            wgpuSamplerRelease(m_pImpl->sampler);
            m_pImpl->sampler = nullptr;
        }
        if (m_pImpl->textureView) {
            wgpuTextureViewRelease(m_pImpl->textureView);
            m_pImpl->textureView = nullptr;
        }
        if (m_pImpl->texture) {
            wgpuTextureDestroy(m_pImpl->texture);
            wgpuTextureRelease(m_pImpl->texture);
            m_pImpl->texture = nullptr;
        }
    }

    WGPUTexture GPUTexture::getTexture() const { return m_pImpl->texture; }
    WGPUTextureView GPUTexture::getTextureView() const { return m_pImpl->textureView; }
    WGPUSampler GPUTexture::getSampler() const { return m_pImpl->sampler; }
    uint32_t GPUTexture::getWidth() const { return m_pImpl->width; }
    uint32_t GPUTexture::getHeight() const { return m_pImpl->height; }

}
