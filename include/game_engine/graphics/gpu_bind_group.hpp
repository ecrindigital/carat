#pragma once

#include <game_engine/core/types.hpp>
#include <memory>
#include <vector>

typedef struct WGPUBindGroupImpl* WGPUBindGroup;
typedef struct WGPUBindGroupLayoutImpl* WGPUBindGroupLayout;

namespace game_engine::graphics {

    class GPUDevice;
    class GPUBuffer;
    class GPUTexture;

    enum class BindingType : uint8_t {
        UniformBuffer,
        Texture,
        Sampler
    };

    struct BindGroupEntry {
        uint32_t binding;
        BindingType type;
        GPUBuffer* buffer = nullptr;
        size_t offset = 0;
        size_t size = 0;
        GPUTexture* texture = nullptr;
    };

    class GPUBindGroup {
    public:
        GPUBindGroup();
        ~GPUBindGroup();

        GPUBindGroup(const GPUBindGroup&) = delete;
        GPUBindGroup& operator=(const GPUBindGroup&) = delete;
        GPUBindGroup(GPUBindGroup&&) noexcept;
        GPUBindGroup& operator=(GPUBindGroup&&) noexcept;

        [[nodiscard]] core::Result initialize(GPUDevice* device, WGPUBindGroupLayout layout, const std::vector<BindGroupEntry>& entries);
        void shutdown();

        [[nodiscard]] WGPUBindGroup getBindGroup() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

    class BindGroupLayoutBuilder {
    public:
        BindGroupLayoutBuilder& addUniformBuffer(uint32_t binding, bool vertex = true, bool fragment = true);
        BindGroupLayoutBuilder& addTexture(uint32_t binding);
        BindGroupLayoutBuilder& addSampler(uint32_t binding);

        [[nodiscard]] WGPUBindGroupLayout build(GPUDevice* device);

    private:
        struct Entry {
            uint32_t binding;
            BindingType type;
            bool vertexVisible = false;
            bool fragmentVisible = false;
        };
        std::vector<Entry> m_entries;
    };

}
