#pragma once

#include <game_engine/graphics/gpu_texture.hpp>
#include <memory>
#include <string>
#include <array>

namespace game_engine::graphics {

    class GPUDevice;

    class TextureLoader {
    public:
        static std::unique_ptr<GPUTexture> load(GPUDevice* device, const std::string& path);
        static std::unique_ptr<GPUTexture> loadHDR(GPUDevice* device, const std::string& path);
        static std::unique_ptr<GPUTexture> loadCubemap(GPUDevice* device,
            const std::array<std::string, 6>& faces);
    };

}
