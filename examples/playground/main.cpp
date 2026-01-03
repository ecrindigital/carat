#include <game_engine/game.hpp>
#include <game_engine/graphics/mesh.hpp>
#include <game_engine/graphics/material.hpp>
#include <game_engine/graphics/gpu_texture.hpp>
#include <game_engine/graphics/lighting.hpp>
#include <game_engine/audio/audio_manager.hpp>
#include <SDL3/SDL_scancode.h>
#include <spdlog/spdlog.h>
#include <vector>
#include <algorithm>
#include <cmath>

namespace {
    constexpr float WORLD_HEIGHT = 12.0f;
    constexpr float WORLD_WIDTH = 16.0f;
    constexpr float PLAYER_SPEED = 8.0f;
    constexpr float PROJECTILE_SPEED = 18.0f;
    constexpr float ENEMY_SPEED = 2.5f;
    constexpr float ENEMY_DROP = 0.5f;
    constexpr float SHOOT_COOLDOWN = 0.2f;

    constexpr int ENEMY_ROWS = 4;
    constexpr int ENEMY_COLS = 8;
    constexpr float ENEMY_SPACING_X = 1.4f;
    constexpr float ENEMY_SPACING_Y = 1.0f;

    const std::string TEXTURES_PATH = "examples/playground/assets/textures/";
    const std::string SOUNDS_PATH = "examples/playground/assets/sounds/";
}

struct Entity {
    game_engine::graphics::Mesh* mesh = nullptr;
    glm::vec3 position{0.0f};
    glm::vec2 size{1.0f, 1.0f};
    bool active = true;

    void updateMeshPosition() {
        if (mesh) mesh->setPosition(position);
    }

    bool collidesWith(const Entity& other) const {
        if (!active || !other.active) return false;
        float halfW1 = size.x * 0.5f, halfH1 = size.y * 0.5f;
        float halfW2 = other.size.x * 0.5f, halfH2 = other.size.y * 0.5f;
        return position.x - halfW1 < other.position.x + halfW2 &&
               position.x + halfW1 > other.position.x - halfW2 &&
               position.y - halfH1 < other.position.y + halfH2 &&
               position.y + halfH1 > other.position.y - halfH2;
    }
};

struct Player : Entity {
    float shootCooldown = 0.0f;
};

struct Enemy : Entity {
    int row = 0;
};

struct Projectile : Entity {
    float velocity = PROJECTILE_SPEED;
    game_engine::graphics::Mesh* glowMesh = nullptr;
    int lightIndex = -1;
};

struct GameState {
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;

    float enemyDirection = 1.0f;
    int score = 0;
    bool gameOver = false;
    bool victory = false;
    float time = 0.0f;
};

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
            enemyBossTex,    
            enemyRedTex,     
            enemyYellowTex,  
            enemyGreenTex    
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

        GameState state;

        state.player.mesh = game.createQuad(0.8f, 0.8f);
        state.player.mesh->setMaterial(playerMat);
        state.player.position = {0.0f, -WORLD_HEIGHT / 2.0f + 1.0f, 0.0f};
        state.player.size = {0.7f, 0.7f};
        state.player.updateMeshPosition();
        game.addMesh(state.player.mesh);

        float startX = -(ENEMY_COLS - 1) * ENEMY_SPACING_X / 2.0f;
        float startY = WORLD_HEIGHT / 2.0f - 2.5f;

        for (int row = 0; row < ENEMY_ROWS; ++row) {
            for (int col = 0; col < ENEMY_COLS; ++col) {
                Enemy enemy;
                enemy.row = row;
                enemy.mesh = game.createQuad(0.6f, 0.6f);
                enemy.mesh->setMaterial(enemyMats[row]);
                enemy.position = {
                    startX + col * ENEMY_SPACING_X,
                    startY - row * ENEMY_SPACING_Y,
                    0.0f
                };
                enemy.size = {0.5f, 0.5f};
                enemy.updateMeshPosition();
                game.addMesh(enemy.mesh);
                state.enemies.push_back(std::move(enemy));
            }
        }

        auto spawnProjectile = [&](const glm::vec3& pos) {
            Projectile proj;
            proj.mesh = game.createQuad(0.1f, 0.2f);
            proj.mesh->setMaterial(projectileMat);
            proj.position = pos;
            proj.size = {0.1f, 0.2f};
            proj.updateMeshPosition();
            game.addMesh(proj.mesh);

            proj.glowMesh = game.createQuad(0.8f, 0.8f);
            proj.glowMesh->setMaterial(glowMat);
            proj.glowMesh->setPosition(pos);
            game.addMesh(proj.glowMesh);

            auto* lighting = game.getLighting();
            if (lighting) {
                proj.lightIndex = lighting->addPointLight(pos, {1.0f, 0.9f, 0.3f}, 1.5f, 2.5f);
            }

            state.projectiles.push_back(std::move(proj));
        };

        spdlog::info("=============================");
        spdlog::info("     SPACE INVADERS 2D");
        spdlog::info("=============================");
        spdlog::info("Controls: LEFT/RIGHT or A/D to move");
        spdlog::info("          SPACE to shoot");
        spdlog::info("Scoring: Top row = 40 pts, Bottom = 10 pts");
        spdlog::info("=============================");

        game.playSound(sfxSpawn, 0.7f);

        game.onUpdate([&](float dt) {
            state.time += dt;

            if (state.gameOver) return;

            float moveX = 0.0f;
            if (game.isKeyPressed(SDL_SCANCODE_LEFT) || game.isKeyPressed(SDL_SCANCODE_A)) {
                moveX = -PLAYER_SPEED * dt;
            }
            if (game.isKeyPressed(SDL_SCANCODE_RIGHT) || game.isKeyPressed(SDL_SCANCODE_D)) {
                moveX = PLAYER_SPEED * dt;
            }

            state.player.position.x += moveX;
            float halfWidth = WORLD_WIDTH / 2.0f - state.player.size.x / 2.0f;
            state.player.position.x = std::clamp(state.player.position.x, -halfWidth, halfWidth);
            state.player.updateMeshPosition();

            state.player.shootCooldown -= dt;
            if (game.isKeyPressed(SDL_SCANCODE_SPACE) && state.player.shootCooldown <= 0.0f) {
                glm::vec3 spawnPos = state.player.position;
                spawnPos.y += state.player.size.y / 2.0f + 0.3f;
                spawnProjectile(spawnPos);
                game.playSound(sfxShoot, 0.5f);
                state.player.shootCooldown = SHOOT_COOLDOWN;
            }

            auto* lighting = game.getLighting();
            for (auto& proj : state.projectiles) {
                if (!proj.active) continue;
                proj.position.y += proj.velocity * dt;
                proj.updateMeshPosition();

                if (proj.glowMesh) {
                    proj.glowMesh->setPosition(proj.position);
                    float glowPulse = 1.0f + std::sin(state.time * 15.0f) * 0.15f;
                    proj.glowMesh->setScale({glowPulse, glowPulse, 1.0f});
                }

                if (lighting && proj.lightIndex >= 0) {
                    lighting->updatePointLight(proj.lightIndex, proj.position);
                }

                if (proj.position.y > WORLD_HEIGHT / 2.0f + 1.0f) {
                    proj.active = false;
                    game.removeMesh(proj.mesh);
                    if (proj.glowMesh) game.removeMesh(proj.glowMesh);
                }
            }

            float maxX = -999.0f, minX = 999.0f;
            int activeCount = 0;

            for (auto& enemy : state.enemies) {
                if (!enemy.active) continue;
                activeCount++;
                enemy.position.x += ENEMY_SPEED * state.enemyDirection * dt;

                if (enemy.position.x > maxX) maxX = enemy.position.x;
                if (enemy.position.x < minX) minX = enemy.position.x;

                float wobble = std::sin(state.time * 4.0f + enemy.position.x * 0.5f) * 0.1f;
                enemy.mesh->setPosition({enemy.position.x, enemy.position.y + wobble, enemy.position.z});
            }

            float bound = WORLD_WIDTH / 2.0f - 1.2f;
            if ((maxX > bound || minX < -bound) && activeCount > 0) {
                state.enemyDirection *= -1.0f;
                for (auto& enemy : state.enemies) {
                    if (enemy.active) {
                        if (enemy.position.x > bound) enemy.position.x = bound;
                        if (enemy.position.x < -bound) enemy.position.x = -bound;
                        enemy.position.y -= ENEMY_DROP;

                        if (enemy.position.y < -WORLD_HEIGHT / 2.0f + 2.0f) {
                            state.gameOver = true;
                            game.playSound(sfxDeath, 0.8f);
                            spdlog::info("=============================");
                            spdlog::info("       GAME OVER!");
                            spdlog::info("   Final Score: {}", state.score);
                            spdlog::info("=============================");
                            return;
                        }
                    }
                }
            }

            for (auto& proj : state.projectiles) {
                if (!proj.active) continue;

                for (auto& enemy : state.enemies) {
                    if (!enemy.active) continue;

                    if (proj.collidesWith(enemy)) {
                        proj.active = false;
                        enemy.active = false;
                        game.removeMesh(proj.mesh);
                        if (proj.glowMesh) game.removeMesh(proj.glowMesh);
                        game.removeMesh(enemy.mesh);

                        int points = (ENEMY_ROWS - enemy.row) * 10;
                        state.score += points;
                        game.playSound(sfxExplosion, 0.6f);
                        spdlog::info("HIT! +{} pts | Total: {}", points, state.score);
                        break;
                    }
                }
            }

            bool anyActive = false;
            for (const auto& e : state.enemies) {
                if (e.active) { anyActive = true; break; }
            }
            if (!anyActive) {
                state.gameOver = true;
                state.victory = true;
                game.playSound(sfxBonus, 0.8f);
                spdlog::info("=============================");
                spdlog::info("       VICTORY!");
                spdlog::info("   Final Score: {}", state.score);
                spdlog::info("=============================");
            }
        });

        game.run();
        return 0;

    } catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }
}
