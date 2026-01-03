#include <game_engine/game.hpp>
#include <game_engine/graphics/mesh.hpp>
#include <game_engine/graphics/material.hpp>
#include <SDL3/SDL_scancode.h>
#include <spdlog/spdlog.h>
#include <vector>
#include <algorithm>

namespace {
    constexpr float WORLD_HEIGHT = 12.0f;
    constexpr float WORLD_WIDTH = 16.0f;
    constexpr float PLAYER_SPEED = 8.0f;
    constexpr float PROJECTILE_SPEED = 15.0f;
    constexpr float ENEMY_SPEED = 2.0f;
    constexpr float ENEMY_DROP = 0.5f;
    constexpr float SHOOT_COOLDOWN = 0.3f;

    constexpr int ENEMY_ROWS = 4;
    constexpr int ENEMY_COLS = 8;
    constexpr float ENEMY_SPACING_X = 1.4f;
    constexpr float ENEMY_SPACING_Y = 1.0f;
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

struct Enemy : Entity {};

struct Projectile : Entity {
    float velocity = PROJECTILE_SPEED;
};

struct GameState {
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Projectile> projectiles;

    float enemyDirection = 1.0f;
    int score = 0;
    bool gameOver = false;
    bool victory = false;
};

int main() {
    try {
        game_engine::Game game("Space Invaders Playground", 800, 600);

        game.setOrthographic(WORLD_HEIGHT, -1.0f, 10.0f);
        game.setCameraPosition({0.0f, 0.0f, 5.0f});
        game.setCameraLookAt({0.0f, 0.0f, 0.0f});
        game.setCameraUp({0.0f, 1.0f, 0.0f});

        auto* playerMat = game.createUnlitMaterial({0.2f, 0.8f, 0.3f, 1.0f});   
        auto* enemyMat = game.createUnlitMaterial({0.9f, 0.2f, 0.2f, 1.0f});    
        auto* projectileMat = game.createUnlitMaterial({1.0f, 1.0f, 0.0f, 1.0f});

        GameState state;

        state.player.mesh = game.createQuad(1.0f, 0.6f);
        state.player.mesh->setMaterial(playerMat);
        state.player.position = {0.0f, -WORLD_HEIGHT / 2.0f + 1.0f, 0.0f};
        state.player.size = {1.0f, 0.6f};
        state.player.updateMeshPosition();
        game.addMesh(state.player.mesh);

        float startX = -(ENEMY_COLS - 1) * ENEMY_SPACING_X / 2.0f;
        float startY = WORLD_HEIGHT / 2.0f - 2.0f;

        for (int row = 0; row < ENEMY_ROWS; ++row) {
            for (int col = 0; col < ENEMY_COLS; ++col) {
                Enemy enemy;
                enemy.mesh = game.createQuad(0.8f, 0.6f);
                enemy.mesh->setMaterial(enemyMat);
                enemy.position = {
                    startX + col * ENEMY_SPACING_X,
                    startY - row * ENEMY_SPACING_Y,
                    0.0f
                };
                enemy.size = {0.8f, 0.6f};
                enemy.updateMeshPosition();
                game.addMesh(enemy.mesh);
                state.enemies.push_back(std::move(enemy));
            }
        }

        auto spawnProjectile = [&](const glm::vec3& pos) {
            Projectile proj;
            proj.mesh = game.createQuad(0.1f, 0.3f);
            proj.mesh->setMaterial(projectileMat);
            proj.position = pos;
            proj.size = {0.1f, 0.3f};
            proj.updateMeshPosition();
            game.addMesh(proj.mesh);
            state.projectiles.push_back(std::move(proj));
        };

        spdlog::info("Game started! Use LEFT/RIGHT or A/D to move, SPACE to shoot.");

        game.onUpdate([&](float dt) {
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
                spawnPos.y += state.player.size.y / 2.0f + 0.2f;
                spawnProjectile(spawnPos);
                state.player.shootCooldown = SHOOT_COOLDOWN;
            }

            for (auto& proj : state.projectiles) {
                if (!proj.active) continue;
                proj.position.y += proj.velocity * dt;
                proj.updateMeshPosition();

                if (proj.position.y > WORLD_HEIGHT / 2.0f + 1.0f) {
                    proj.active = false;
                    game.removeMesh(proj.mesh);
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

                enemy.updateMeshPosition();
            }

            float bound = WORLD_WIDTH / 2.0f - 1.0f;
            if (maxX > bound || minX < -bound) {
                state.enemyDirection *= -1.0f;
                for (auto& enemy : state.enemies) {
                    if (enemy.active) {
                        enemy.position.y -= ENEMY_DROP;
                        enemy.updateMeshPosition();

                        if (enemy.position.y < -WORLD_HEIGHT / 2.0f + 2.0f) {
                            state.gameOver = true;
                            spdlog::info("GAME OVER! Final Score: {}", state.score);
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
                        game.removeMesh(enemy.mesh);
                        state.score += 10;
                        spdlog::info("Hit! Score: {}", state.score);
                        break;
                    }
                }
            }

            if (activeCount == 0 || (activeCount == 1 && state.enemies.back().active == false)) {
                bool anyActive = false;
                for (const auto& e : state.enemies) {
                    if (e.active) { anyActive = true; break; }
                }
                if (!anyActive) {
                    state.gameOver = true;
                    state.victory = true;
                    spdlog::info("YOU WIN! Final Score: {}", state.score);
                }
            }
        });

        game.run();
        return 0;

    } catch (const std::exception& e) {
        spdlog::error("Error: {}", e.what());
        return 1;
    }
}
