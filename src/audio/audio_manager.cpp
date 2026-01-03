#include <game_engine/audio/audio_manager.hpp>
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>

namespace game_engine::audio {

    class AudioClip::Impl {
    public:
        std::string path;
        SDL_AudioSpec spec{};
        uint8_t* buffer = nullptr;
        uint32_t length = 0;
        bool loaded = false;
    };

    AudioClip::AudioClip() : m_pImpl(std::make_unique<Impl>()) {}
    AudioClip::~AudioClip() {
        if (m_pImpl && m_pImpl->buffer) {
            SDL_free(m_pImpl->buffer);
        }
    }

    AudioClip::AudioClip(AudioClip&&) noexcept = default;
    AudioClip& AudioClip::operator=(AudioClip&&) noexcept = default;

    core::Result AudioClip::load(const std::string& path) {
        m_pImpl->path = path;

        if (!SDL_LoadWAV(path.c_str(), &m_pImpl->spec, &m_pImpl->buffer, &m_pImpl->length)) {
            spdlog::error("Failed to load audio: {} - {}", path, SDL_GetError());
            return core::Result::Error;
        }

        m_pImpl->loaded = true;
        spdlog::info("Loaded audio: {} ({} bytes)", path, m_pImpl->length);
        return core::Result::Success;
    }

    bool AudioClip::isLoaded() const { return m_pImpl->loaded; }
    const std::string& AudioClip::getPath() const { return m_pImpl->path; }
    void* AudioClip::getData() const { return m_pImpl->buffer; }
    uint32_t AudioClip::getLength() const { return m_pImpl->length; }
    void* AudioClip::getSpec() const { return &m_pImpl->spec; }

    class AudioManager::Impl {
    public:
        SDL_AudioDeviceID deviceId = 0;
        SDL_AudioSpec deviceSpec{};
        float masterVolume = 1.0f;
        std::unordered_map<std::string, std::unique_ptr<AudioClip>> clips;
        bool initialized = false;
    };

    AudioManager::AudioManager() : m_pImpl(std::make_unique<Impl>()) {}
    AudioManager::~AudioManager() { shutdown(); }

    AudioManager::AudioManager(AudioManager&&) noexcept = default;
    AudioManager& AudioManager::operator=(AudioManager&&) noexcept = default;

    core::Result AudioManager::initialize() {
        if (m_pImpl->initialized) return core::Result::Success;

        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
            spdlog::error("Failed to initialize SDL audio: {}", SDL_GetError());
            return core::Result::Error;
        }

        SDL_AudioSpec wantSpec{};
        wantSpec.freq = 44100;
        wantSpec.format = SDL_AUDIO_S16;
        wantSpec.channels = 2;

        m_pImpl->deviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &wantSpec);
        if (m_pImpl->deviceId == 0) {
            spdlog::error("Failed to open audio device: {}", SDL_GetError());
            return core::Result::Error;
        }

        m_pImpl->deviceSpec = wantSpec;
        m_pImpl->initialized = true;
        spdlog::info("Audio initialized (device: {})", m_pImpl->deviceId);
        return core::Result::Success;
    }

    void AudioManager::shutdown() {
        if (!m_pImpl) return;
        if (m_pImpl->deviceId != 0) {
            SDL_CloseAudioDevice(m_pImpl->deviceId);
            m_pImpl->deviceId = 0;
        }
        m_pImpl->clips.clear();
        if (m_pImpl->initialized) {
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
        }
        m_pImpl->initialized = false;
    }

    AudioClip* AudioManager::loadClip(const std::string& path) {
        auto it = m_pImpl->clips.find(path);
        if (it != m_pImpl->clips.end()) {
            return it->second.get();
        }

        auto clip = std::make_unique<AudioClip>();
        if (clip->load(path) != core::Result::Success) {
            return nullptr;
        }

        auto* ptr = clip.get();
        m_pImpl->clips[path] = std::move(clip);
        return ptr;
    }

    void AudioManager::play(AudioClip* clip, float volume) {
        if (!clip || !clip->isLoaded() || !m_pImpl->initialized) return;

        auto* srcSpec = static_cast<SDL_AudioSpec*>(clip->getSpec());

        SDL_AudioStream* stream = SDL_CreateAudioStream(srcSpec, &m_pImpl->deviceSpec);
        if (!stream) {
            spdlog::warn("Failed to create audio stream: {}", SDL_GetError());
            return;
        }

        float finalVolume = volume * m_pImpl->masterVolume;
        SDL_SetAudioStreamGain(stream, finalVolume);

        if (!SDL_PutAudioStreamData(stream, clip->getData(), static_cast<int>(clip->getLength()))) {
            spdlog::warn("Failed to put audio data: {}", SDL_GetError());
            SDL_DestroyAudioStream(stream);
            return;
        }

        if (!SDL_BindAudioStream(m_pImpl->deviceId, stream)) {
            spdlog::warn("Failed to bind audio stream: {}", SDL_GetError());
            SDL_DestroyAudioStream(stream);
            return;
        }
    }

    void AudioManager::playOneShot(const std::string& path, float volume) {
        auto* clip = loadClip(path);
        if (clip) {
            play(clip, volume);
        }
    }

    void AudioManager::setMasterVolume(float volume) {
        m_pImpl->masterVolume = std::clamp(volume, 0.0f, 1.0f);
    }

    float AudioManager::getMasterVolume() const {
        return m_pImpl->masterVolume;
    }

}
