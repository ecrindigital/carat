#pragma once

#include <game_engine/core/types.hpp>
#include <string>
#include <memory>
#include <unordered_map>

namespace game_engine::audio {

    class AudioClip;

    class AudioManager {
    public:
        AudioManager();
        ~AudioManager();

        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;
        AudioManager(AudioManager&&) noexcept;
        AudioManager& operator=(AudioManager&&) noexcept;

        [[nodiscard]] core::Result initialize();
        void shutdown();

        AudioClip* loadClip(const std::string& path);
        void play(AudioClip* clip, float volume = 1.0f);
        void playOneShot(const std::string& path, float volume = 1.0f);

        void setMasterVolume(float volume);
        [[nodiscard]] float getMasterVolume() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

    class AudioClip {
    public:
        AudioClip();
        ~AudioClip();

        AudioClip(const AudioClip&) = delete;
        AudioClip& operator=(const AudioClip&) = delete;
        AudioClip(AudioClip&&) noexcept;
        AudioClip& operator=(AudioClip&&) noexcept;

        [[nodiscard]] core::Result load(const std::string& path);
        [[nodiscard]] bool isLoaded() const;
        [[nodiscard]] const std::string& getPath() const;

        void* getData() const;
        uint32_t getLength() const;
        void* getSpec() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_pImpl;
    };

}
