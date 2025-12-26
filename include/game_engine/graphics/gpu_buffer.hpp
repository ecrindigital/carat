#pragma once

#include <game_engine/core/types.hpp>
#include <memory>
#include <span>
#include <cstdint>

typedef struct WGPUBufferImpl* WGPUBuffer;

namespace game_engine::graphics {

    class GPUDevice;

    enum class BufferUsage : uint32_t {
        Vertex = 0x0020,
        Index = 0x0010,
        Uniform = 0x0040,
        Storage = 0x0080,
        CopySrc = 0x0004,
        CopyDst = 0x0008
    };

    inline BufferUsage operator|(BufferUsage a, BufferUsage b) {
        return static_cast<BufferUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    class GPUBuffer {
    public:
        GPUBuffer();
        ~GPUBuffer();

        GPUBuffer(const GPUBuffer&) = delete;
        GPUBuffer& operator=(const GPUBuffer&) = delete;
        GPUBuffer(GPUBuffer&&) noexcept;
        GPUBuffer& operator=(GPUBuffer&&) noexcept;

        [[nodiscard]] core::Result initialize(GPUDevice* device, size_t size, BufferUsage usage);

        template<typename T>
        [[nodiscard]] core::Result initializeWithData(GPUDevice* device, std::span<const T> data, BufferUsage usage) {
            auto result = initialize(device, data.size_bytes(), usage);
            if (result == core::Result::Success) {
                write(data.data(), data.size_bytes());
            }
            return result;
        }

        void write(const void* data, size_t size, size_t offset = 0);
        void shutdown();

        [[nodiscard]] WGPUBuffer getBuffer() const;
        [[nodiscard]] size_t getSize() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

} // namespace game_engine::graphics
