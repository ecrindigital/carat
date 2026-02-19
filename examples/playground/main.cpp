#include <game_engine/game.hpp>
#include <game_engine/domain/system.hpp>
#include <game_engine/graphics/mesh.hpp>
#include <game_engine/graphics/material.hpp>
#include <game_engine/graphics/gpu_texture.hpp>
#include <game_engine/graphics/lighting.hpp>
#include <game_engine/audio/audio_manager.hpp>
#include <spdlog/spdlog.h>

namespace {
    using namespace game_engine::domain;

    constexpr int ENEMY_ROWS = 4;
    constexpr int ENEMY_COLS = 8;
    constexpr float ENEMY_SPACING_X = 1.4f;
    constexpr float ENEMY_SPACING_Y = 1.0f;
    constexpr float WORLD_HEIGHT = 12.0f;

    const std::string TEXTURES_PATH = "examples/playground/assets/textures/";
    const std::string SOUNDS_PATH = "examples/playground/assets/sounds/";
}

int main() {
    try {
        game_engine::Game game("Space Invaders 2D", 1024, 768);

        game.setOrthographic(WORLD_HEIGHT, 0.1f, 100.0f);
        game.setCameraPosition({0.0f, 0.0f, 10.0f});
        game.setCameraLookAt({0.0f, 0.0f, 0.0f});
        game.setCameraUp({0.0f, 1.0f, 0.0f});

        if (game.initialize() != game_engine::core::Result::Success) {
            spdlog::error("Failed to initialize game");
            return 1;
        }

        auto* playerTex = game.loadTexture(TEXTURES_PATH + "player.png");
        auto* enemyBossTex = game.loadTexture(TEXTURES_PATH + "enemy_boss.png");
        auto* enemyRedTex = game.loadTexture(TEXTURES_PATH + "enemy_red.png");
        auto* enemyYellowTex = game.loadTexture(TEXTURES_PATH + "enemy_yellow.png");
        auto* enemyGreenTex = game.loadTexture(TEXTURES_PATH + "enemy_green.png");

        auto* sfxShoot = game.loadAudio(SOUNDS_PATH + "siclone_shoot.wav");
        auto* sfxExplosion = game.loadAudio(SOUNDS_PATH + "siclone_explosion_small.wav");
        auto* sfxSpawn = game.loadAudio(SOUNDS_PATH + "siclone_spawn.wav");
        auto* sfxDeath = game.loadAudio(SOUNDS_PATH + "siclone_death.wav");
        auto* sfxBonus = game.loadAudio(SOUNDS_PATH + "siclone_bonus.wav");

        auto* playerMat = game.createSpriteMaterial(playerTex);

        std::vector<game_engine::graphics::Material*> enemyMats;
        game_engine::graphics::GPUTexture* enemyTextures[] = {
            enemyBossTex, enemyRedTex, enemyYellowTex, enemyGreenTex
        };
        for (int i = 0; i < ENEMY_ROWS; ++i) {
            enemyMats.push_back(game.createSpriteMaterial(enemyTextures[i]));
        }

        auto* projectileMat = game.createUnlitMaterial({1.0f, 1.0f, 0.5f, 1.0f});
        auto* glowMat = game.createUnlitMaterial({1.0f, 0.8f, 0.2f, 0.4f});

        game.setAmbientLight({0.1f, 0.1f, 0.15f}, 1.0f);

        auto* bgMat = game.createUnlitMaterial({0.02f, 0.02f, 0.05f, 1.0f});
        auto* background = game.createQuad(30.0f, 24.0f);
        background->setMaterial(bgMat);
        background->setPosition({0.0f, 0.0f, -5.0f});
        game.addMesh(background);

        auto& world = game.world();

        world.set<components::GameRef>({&game});
        world.set<components::GameState>({});
        world.set<components::AudioAssets>({sfxShoot, sfxExplosion, sfxSpawn, sfxDeath, sfxBonus});
        world.set<components::SpawnContext>({projectileMat, glowMat});

        auto* playerMesh = game.createQuad(0.8f, 0.8f);
        playerMesh->setMaterial(playerMat);
        playerMesh->setPosition({0.0f, -WORLD_HEIGHT / 2.0f + 1.0f, 0.0f});
        game.addMesh(playerMesh);

        world.entity("Player")
            .set(components::Position{{0.0f, -WORLD_HEIGHT / 2.0f + 1.0f, 0.0f}})
            .set(components::CollisionBox{{0.7f, 0.7f}})
            .set(components::ShootCooldown{})
            .set(components::MeshRef{playerMesh})
            .add<components::Player>();

        float startX = -(ENEMY_COLS - 1) * ENEMY_SPACING_X / 2.0f;
        float startY = WORLD_HEIGHT / 2.0f - 2.5f;

        for (int row = 0; row < ENEMY_ROWS; ++row) {
            for (int col = 0; col < ENEMY_COLS; ++col) {
                auto* enemyMesh = game.createQuad(0.6f, 0.6f);
                enemyMesh->setMaterial(enemyMats[row]);
                glm::vec3 pos = {
                    startX + col * ENEMY_SPACING_X,
                    startY - row * ENEMY_SPACING_Y,
                    0.0f
                };
                enemyMesh->setPosition(pos);
                game.addMesh(enemyMesh);

                world.entity()
                    .set(components::Position{pos})
                    .set(components::CollisionBox{{0.5f, 0.5f}})
                    .set(components::EnemyRow{row})
                    .set(components::MeshRef{enemyMesh})
                    .add<components::Enemy>();
            }
        }

        systems::registerTimeSystem(world);
        systems::registerInputSystem(world);
        systems::registerMovementSystem(world);
        systems::registerEnemyMoveSystem(world);
        systems::registerEnemyBounceSystem(world);
        systems::registerEnemyWobbleSystem(world);
        systems::registerCollisionSystem(world);
        systems::registerProjectileCleanupSystem(world);
        systems::registerVictorySystem(world);
        systems::registerGlowPulseSystem(world);
        systems::registerMeshSyncSystem(world);
        systems::registerPointLightSyncSystem(world);

        spdlog::info("=============================");
        spdlog::info("     SPACE INVADERS 2D");
        spdlog::info("=============================");
        spdlog::info("Controls: LEFT/RIGHT or A/D to move");
        spdlog::info("          SPACE to shoot");
        spdlog::info("Scoring: Top row = 40 pts, Bottom = 10 pts");
        spdlog::info("=============================");

        game.playSound(sfxSpawn, 0.7f);
        game.run();
        return 0;

    } catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }
}
