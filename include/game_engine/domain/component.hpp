#pragma once
#include <glm/glm.hpp>

namespace game_engine { class Game; }
namespace game_engine::graphics { class Mesh; class Material; }
namespace game_engine::audio { class AudioClip; }

namespace game_engine::domain::components {

    struct Position {
        glm::vec3 value;
    };

    struct Velocity {
        glm::vec3 value;
    };

    struct ShootCooldown {
        float remaining = 0.0f;
    };

    struct EnemyRow {
        int row = 0;
    };

    struct CollisionBox {
        glm::vec2 size;
    };

    struct MeshRef {
        graphics::Mesh* mesh = nullptr;
    };

    struct GlowEffect {
        graphics::Mesh* glowMesh = nullptr;
    };

    struct Player {};
    struct Enemy {};
    struct Projectile {};

    struct GameRef {
        Game* game = nullptr;
    };

    struct GameState {
        float enemyDirection = 1.0f;
        float time = 0.0f;
        float enemyMinX = 0.0f;
        float enemyMaxX = 0.0f;
        int activeEnemyCount = 0;
        int score = 0;
        bool gameOver = false;
        bool victory = false;
    };

    struct AudioAssets {
        audio::AudioClip* shoot = nullptr;
        audio::AudioClip* explosion = nullptr;
        audio::AudioClip* spawn = nullptr;
        audio::AudioClip* death = nullptr;
        audio::AudioClip* bonus = nullptr;
    };

    struct SpawnContext {
        graphics::Material* projectileMat = nullptr;
        graphics::Material* glowMat = nullptr;
    };

} // namespace game_engine::domain::components
