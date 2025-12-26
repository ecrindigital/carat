#include <game_engine/graphics/gpu_buffer.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <webgpu/webgpu.h>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    class GPUBuffer::Impl {
    public:
        WGPUBuffer buffer = nullptr;
        WGPUDevice device = nullptr;
        WGPUQueue queue = nullptr;
        size_t size = 0;
    };

    GPUBuffer::GPUBuffer() : m_pImpl(std::make_unique<Impl>()) {}

    GPUBuffer::~GPUBuffer() {
        shutdown();
    }

    GPUBuffer::GPUBuffer(GPUBuffer&&) noexcept = default;
    GPUBuffer& GPUBuffer::operator=(GPUBuffer&&) noexcept = default;

    core::Result GPUBuffer::initialize(GPUDevice* device, size_t size, BufferUsage usage) {
        m_pImpl->device = device->getDevice();
        m_pImpl->queue = device->getQueue();
        m_pImpl->size = size;

        WGPUBufferDescriptor bufferDesc = {};
        bufferDesc.size = size;
        bufferDesc.usage = static_cast<WGPUBufferUsageFlags>(usage) | WGPUBufferUsage_CopyDst;
        bufferDesc.mappedAtCreation = false;

        m_pImpl->buffer = wgpuDeviceCreateBuffer(m_pImpl->device, &bufferDesc);
        if (!m_pImpl->buffer) {
            spdlog::error("Failed to create WebGPU buffer");
            return core::Result::Error;
        }

        return core::Result::Success;
    }

    void GPUBuffer::write(const void* data, size_t size, size_t offset) {
        if (m_pImpl->buffer && m_pImpl->queue) {
            wgpuQueueWriteBuffer(m_pImpl->queue, m_pImpl->buffer, offset, data, size);
        }
    }

    void GPUBuffer::shutdown() {
        if (m_pImpl->buffer) {
            wgpuBufferRelease(m_pImpl->buffer);
            m_pImpl->buffer = nullptr;
        }
    }

    WGPUBuffer GPUBuffer::getBuffer() const { return m_pImpl->buffer; }
    size_t GPUBuffer::getSize() const { return m_pImpl->size; }

} // namespace game_engine::graphics
