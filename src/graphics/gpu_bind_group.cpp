#include <game_engine/graphics/gpu_bind_group.hpp>
#include <game_engine/graphics/gpu_device.hpp>
#include <game_engine/graphics/gpu_buffer.hpp>
#include <game_engine/graphics/gpu_texture.hpp>
#include <webgpu/webgpu.h>
#include <spdlog/spdlog.h>

namespace game_engine::graphics {

    class GPUBindGroup::Impl {
    public:
        WGPUBindGroup bindGroup = nullptr;
        WGPUDevice device = nullptr;
    };

    GPUBindGroup::GPUBindGroup() : m_pImpl(std::make_unique<Impl>()) {}

    GPUBindGroup::~GPUBindGroup() {
        shutdown();
    }

    GPUBindGroup::GPUBindGroup(GPUBindGroup&&) noexcept = default;
    GPUBindGroup& GPUBindGroup::operator=(GPUBindGroup&&) noexcept = default;

    core::Result GPUBindGroup::initialize(GPUDevice* device, WGPUBindGroupLayout layout, const std::vector<BindGroupEntry>& entries) {
        m_pImpl->device = device->getDevice();

        std::vector<WGPUBindGroupEntry> wgpuEntries;
        wgpuEntries.reserve(entries.size());

        for (const auto& entry : entries) {
            WGPUBindGroupEntry wgpuEntry = {};
            wgpuEntry.binding = entry.binding;

            switch (entry.type) {
                case BindingType::UniformBuffer:
                    if (entry.buffer) {
                        wgpuEntry.buffer = entry.buffer->getBuffer();
                        wgpuEntry.offset = entry.offset;
                        wgpuEntry.size = entry.size > 0 ? entry.size : entry.buffer->getSize();
                    }
                    break;

                case BindingType::Texture:
                    if (entry.texture) {
                        wgpuEntry.textureView = entry.texture->getTextureView();
                    }
                    break;

                case BindingType::Sampler:
                    if (entry.texture) {
                        wgpuEntry.sampler = entry.texture->getSampler();
                    }
                    break;
            }

            wgpuEntries.push_back(wgpuEntry);
        }

        WGPUBindGroupDescriptor bindGroupDesc = {};
        bindGroupDesc.layout = layout;
        bindGroupDesc.entryCount = static_cast<uint32_t>(wgpuEntries.size());
        bindGroupDesc.entries = wgpuEntries.data();

        m_pImpl->bindGroup = wgpuDeviceCreateBindGroup(m_pImpl->device, &bindGroupDesc);
        if (!m_pImpl->bindGroup) {
            spdlog::error("Failed to create bind group");
            return core::Result::Error;
        }

        return core::Result::Success;
    }

    void GPUBindGroup::shutdown() {
        if (m_pImpl->bindGroup) {
            wgpuBindGroupRelease(m_pImpl->bindGroup);
            m_pImpl->bindGroup = nullptr;
        }
    }

    WGPUBindGroup GPUBindGroup::getBindGroup() const { return m_pImpl->bindGroup; }

    BindGroupLayoutBuilder& BindGroupLayoutBuilder::addUniformBuffer(uint32_t binding, bool vertex, bool fragment) {
        m_entries.push_back({binding, BindingType::UniformBuffer, vertex, fragment});
        return *this;
    }

    BindGroupLayoutBuilder& BindGroupLayoutBuilder::addTexture(uint32_t binding) {
        m_entries.push_back({binding, BindingType::Texture, false, true});
        return *this;
    }

    BindGroupLayoutBuilder& BindGroupLayoutBuilder::addSampler(uint32_t binding) {
        m_entries.push_back({binding, BindingType::Sampler, false, true});
        return *this;
    }

    WGPUBindGroupLayout BindGroupLayoutBuilder::build(GPUDevice* device) {
        std::vector<WGPUBindGroupLayoutEntry> layoutEntries;
        layoutEntries.reserve(m_entries.size());

        for (const auto& entry : m_entries) {
            WGPUBindGroupLayoutEntry layoutEntry = {};
            layoutEntry.binding = entry.binding;

            WGPUShaderStageFlags visibility = 0;
            if (entry.vertexVisible) visibility |= WGPUShaderStage_Vertex;
            if (entry.fragmentVisible) visibility |= WGPUShaderStage_Fragment;
            layoutEntry.visibility = visibility;

            switch (entry.type) {
                case BindingType::UniformBuffer:
                    layoutEntry.buffer.type = WGPUBufferBindingType_Uniform;
                    layoutEntry.buffer.hasDynamicOffset = false;
                    layoutEntry.buffer.minBindingSize = 0;
                    break;

                case BindingType::Texture:
                    layoutEntry.texture.sampleType = WGPUTextureSampleType_Float;
                    layoutEntry.texture.viewDimension = WGPUTextureViewDimension_2D;
                    layoutEntry.texture.multisampled = false;
                    break;

                case BindingType::Sampler:
                    layoutEntry.sampler.type = WGPUSamplerBindingType_Filtering;
                    break;
            }

            layoutEntries.push_back(layoutEntry);
        }

        WGPUBindGroupLayoutDescriptor layoutDesc = {};
        layoutDesc.entryCount = static_cast<uint32_t>(layoutEntries.size());
        layoutDesc.entries = layoutEntries.data();

        return wgpuDeviceCreateBindGroupLayout(device->getDevice(), &layoutDesc);
    }

}
