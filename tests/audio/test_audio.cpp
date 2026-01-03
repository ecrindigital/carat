#include <catch2/catch_all.hpp>
#include <game_engine/audio/audio_manager.hpp>

using namespace game_engine::audio;
using namespace game_engine::core;

TEST_CASE("AudioClip construction") {
    SECTION("given new AudioClip when constructed then not loaded") {
        AudioClip clip;
        REQUIRE_FALSE(clip.isLoaded());
    }

    SECTION("given new AudioClip when constructed then path is empty") {
        AudioClip clip;
        REQUIRE(clip.getPath().empty());
    }

    SECTION("given new AudioClip when constructed then data is null") {
        AudioClip clip;
        REQUIRE(clip.getData() == nullptr);
    }

    SECTION("given new AudioClip when constructed then length is 0") {
        AudioClip clip;
        REQUIRE(clip.getLength() == 0);
    }
}

TEST_CASE("AudioClip load error cases") {
    SECTION("given AudioClip when loading nonexistent file then returns Error") {
        AudioClip clip;
        auto result = clip.load("/nonexistent/path/audio.wav");
        REQUIRE(result == Result::Error);
    }

    SECTION("given AudioClip when loading nonexistent file then not marked as loaded") {
        AudioClip clip;
        [[maybe_unused]] auto result = clip.load("/nonexistent/path/audio.wav");
        REQUIRE_FALSE(clip.isLoaded());
    }

    SECTION("given AudioClip when loading nonexistent file then path is still set") {
        AudioClip clip;
        [[maybe_unused]] auto result = clip.load("/some/path/audio.wav");
        REQUIRE(clip.getPath() == "/some/path/audio.wav");
    }

    SECTION("given AudioClip when loading invalid path then data remains null") {
        AudioClip clip;
        [[maybe_unused]] auto result = clip.load("/invalid/path.wav");
        REQUIRE(clip.getData() == nullptr);
    }

    SECTION("given AudioClip when loading empty path then returns Error") {
        AudioClip clip;
        auto result = clip.load("");
        REQUIRE(result == Result::Error);
    }
}


TEST_CASE("AudioManager construction") {
    SECTION("given new AudioManager when constructed then master volume is 1") {
        AudioManager manager;
        REQUIRE(manager.getMasterVolume() == Catch::Approx(1.0f));
    }
}

TEST_CASE("AudioManager master volume") {
    SECTION("given AudioManager when setting volume to 0.5 then returns 0.5") {
        AudioManager manager;
        manager.setMasterVolume(0.5f);
        REQUIRE(manager.getMasterVolume() == Catch::Approx(0.5f));
    }

    SECTION("given AudioManager when setting volume to 0 then returns 0") {
        AudioManager manager;
        manager.setMasterVolume(0.0f);
        REQUIRE(manager.getMasterVolume() == Catch::Approx(0.0f));
    }

    SECTION("given AudioManager when setting volume to 1 then returns 1") {
        AudioManager manager;
        manager.setMasterVolume(1.0f);
        REQUIRE(manager.getMasterVolume() == Catch::Approx(1.0f));
    }

    SECTION("given AudioManager when setting volume above 1 then clamps to 1") {
        AudioManager manager;
        manager.setMasterVolume(2.0f);
        REQUIRE(manager.getMasterVolume() == Catch::Approx(1.0f));
    }

    SECTION("given AudioManager when setting volume below 0 then clamps to 0") {
        AudioManager manager;
        manager.setMasterVolume(-0.5f);
        REQUIRE(manager.getMasterVolume() == Catch::Approx(0.0f));
    }

    SECTION("given AudioManager when setting volume multiple times then last value retained") {
        AudioManager manager;
        manager.setMasterVolume(0.2f);
        manager.setMasterVolume(0.8f);
        manager.setMasterVolume(0.5f);
        REQUIRE(manager.getMasterVolume() == Catch::Approx(0.5f));
    }
}


TEST_CASE("AudioManager without initialization") {
    SECTION("given uninitialized AudioManager when loading clip then returns nullptr") {
        AudioManager manager;
        auto* clip = manager.loadClip("/nonexistent/path.wav");
        REQUIRE(clip == nullptr);
    }

    SECTION("given uninitialized AudioManager when calling play with nullptr then no crash") {
        AudioManager manager;
        REQUIRE_NOTHROW(manager.play(nullptr, 1.0f));
    }

    SECTION("given uninitialized AudioManager when calling playOneShot then no crash") {
        AudioManager manager;
        REQUIRE_NOTHROW(manager.playOneShot("/nonexistent.wav", 1.0f));
    }

    SECTION("given uninitialized AudioManager when shutdown then no crash") {
        AudioManager manager;
        REQUIRE_NOTHROW(manager.shutdown());
    }

    SECTION("given uninitialized AudioManager when shutdown called twice then no crash") {
        AudioManager manager;
        manager.shutdown();
        REQUIRE_NOTHROW(manager.shutdown());
    }
}

TEST_CASE("Result enum") {
    SECTION("given Success result when checking then is Success") {
        Result result = Result::Success;
        REQUIRE(result == Result::Success);
    }

    SECTION("given Error result when checking then is Error") {
        Result result = Result::Error;
        REQUIRE(result == Result::Error);
    }

    SECTION("given Success when cast to int then is 0") {
        REQUIRE(static_cast<int>(Result::Success) == 0);
    }

    SECTION("given Error when cast to int then is 1") {
        REQUIRE(static_cast<int>(Result::Error) == 1);
    }
}
