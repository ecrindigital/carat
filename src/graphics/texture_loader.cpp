#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <game_engine/graphics/texture_loader.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    std::unique_ptr<GPUTexture> TextureLoader::load(GPUDevice* device, const std::string& path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

        if (!data) {
            spdlog::error("Failed to load texture: {}", path);
            return nullptr;
        }

        auto texture = std::make_unique<GPUTexture>();
        if (texture->initializeFromData(device, data,
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height), 4) != core::Result::Success) {
            spdlog::error("Failed to create GPU texture from: {}", path);
            stbi_image_free(data);
            return nullptr;
        }

        stbi_image_free(data);
        spdlog::info("Loaded texture: {} ({}x{})", path, width, height);
        return texture;
    }

    std::unique_ptr<GPUTexture> TextureLoader::loadHDR(GPUDevice* device, const std::string& path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        float* data = stbi_loadf(path.c_str(), &width, &height, &channels, 4);

        if (!data) {
            spdlog::error("Failed to load HDR texture: {}", path);
            return nullptr;
        }

        auto texture = std::make_unique<GPUTexture>();
        if (texture->initializeFromHDRData(device, data,
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)) != core::Result::Success) {
            spdlog::error("Failed to create HDR GPU texture from: {}", path);
            stbi_image_free(data);
            return nullptr;
        }

        stbi_image_free(data);
        spdlog::info("Loaded HDR texture: {} ({}x{})", path, width, height);
        return texture;
    }

    std::unique_ptr<GPUTexture> TextureLoader::loadCubemap(GPUDevice* device,
            const std::array<std::string, 6>& faces) {
        std::array<unsigned char*, 6> faceData{};
        int width = 0, height = 0;

        stbi_set_flip_vertically_on_load(false);

        for (size_t i = 0; i < 6; ++i) {
            int w, h, channels;
            faceData[i] = stbi_load(faces[i].c_str(), &w, &h, &channels, 4);

            if (!faceData[i]) {
                spdlog::error("Failed to load cubemap face: {}", faces[i]);
                for (size_t j = 0; j < i; ++j) {
                    stbi_image_free(faceData[j]);
                }
                return nullptr;
            }

            if (i == 0) {
                width = w;
                height = h;
            } else if (w != width || h != height) {
                spdlog::error("Cubemap face size mismatch: {}", faces[i]);
                for (size_t j = 0; j <= i; ++j) {
                    stbi_image_free(faceData[j]);
                }
                return nullptr;
            }
        }

        auto texture = std::make_unique<GPUTexture>();
        if (texture->initializeCubemap(device, faceData,
                static_cast<uint32_t>(width)) != core::Result::Success) {
            spdlog::error("Failed to create cubemap texture");
            for (auto& data : faceData) {
                stbi_image_free(data);
            }
            return nullptr;
        }

        for (auto& data : faceData) {
            stbi_image_free(data);
        }

        spdlog::info("Loaded cubemap ({}x{})", width, height);
        return texture;
    }

}
