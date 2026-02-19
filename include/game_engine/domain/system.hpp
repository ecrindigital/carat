#pragma once

#include <flecs.h>
#include "component.hpp"
#include <game_engine/game.hpp>
#include <game_engine/graphics/mesh.hpp>
#include <game_engine/graphics/material.hpp>
#include <game_engine/audio/audio_manager.hpp>
#include <SDL3/SDL_scancode.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

namespace game_engine::domain::systems {

    namespace constants {
        constexpr float WORLD_HEIGHT = 12.0f;
        constexpr float WORLD_WIDTH = 16.0f;
        constexpr float PLAYER_SPEED = 8.0f;
        constexpr float PROJECTILE_SPEED = 18.0f;
        constexpr float ENEMY_SPEED = 2.5f;
        constexpr float ENEMY_DROP = 0.5f;
        constexpr float SHOOT_COOLDOWN = 0.2f;
        constexpr int ENEMY_ROWS = 4;
    }

    inline void registerMovementSystem(flecs::world& world) {
        world.system<components::Position, const components::Velocity>("MovementSystem")
            .kind(flecs::OnUpdate)
            .each([](flecs::iter& it, size_t, components::Position& pos, const components::Velocity& vel) {
                pos.value += vel.value * it.delta_time();
            });
    }

    inline void registerTimeSystem(flecs::world& world) {
        world.system("TimeSystem")
            .kind(flecs::PreUpdate)
            .run([](flecs::iter& it) {
                auto* gs = it.world().get_mut<components::GameState>();
                gs->time += it.delta_time();
                while (it.next()) {}
            });
    }

    inline void registerInputSystem(flecs::world& world) {
        world.system<components::Position, components::ShootCooldown, const components::CollisionBox>("InputSystem")
            .with<components::Player>()
            .kind(flecs::PreUpdate)
            .each([](flecs::iter& it, size_t, components::Position& pos, components::ShootCooldown& cd, const components::CollisionBox& box) {
                auto* gs = it.world().get_mut<components::GameState>();
                if (gs->gameOver) return;

                auto* game = it.world().get<components::GameRef>()->game;
                float dt = it.delta_time();

                float moveX = 0.0f;
                if (game->isKeyPressed(SDL_SCANCODE_LEFT) || game->isKeyPressed(SDL_SCANCODE_A))
                    moveX = -constants::PLAYER_SPEED * dt;
                if (game->isKeyPressed(SDL_SCANCODE_RIGHT) || game->isKeyPressed(SDL_SCANCODE_D))
                    moveX = constants::PLAYER_SPEED * dt;

                pos.value.x += moveX;
                float halfWidth = constants::WORLD_WIDTH / 2.0f - box.size.x / 2.0f;
                pos.value.x = std::clamp(pos.value.x, -halfWidth, halfWidth);

                cd.remaining -= dt;
                if (game->isKeyPressed(SDL_SCANCODE_SPACE) && cd.remaining <= 0.0f) {
                    auto* sc = it.world().get<components::SpawnContext>();
                    auto* audio = it.world().get<components::AudioAssets>();

                    glm::vec3 spawnPos = pos.value;
                    spawnPos.y += box.size.y / 2.0f + 0.3f;

                    auto* projMesh = game->createQuad(0.1f, 0.2f);
                    projMesh->setMaterial(sc->projectileMat);
                    game->addMesh(projMesh);

                    auto* glowMesh = game->createQuad(0.8f, 0.8f);
                    glowMesh->setMaterial(sc->glowMat);
                    game->addMesh(glowMesh);

                    it.world().entity()
                        .set(components::Position{spawnPos})
                        .set(components::Velocity{{0.0f, constants::PROJECTILE_SPEED, 0.0f}})
                        .set(components::CollisionBox{{0.1f, 0.2f}})
                        .set(components::MeshRef{projMesh})
                        .set(components::GlowEffect{glowMesh})
                        .add<components::Projectile>();

                    game->playSound(audio->shoot, 0.5f);
                    cd.remaining = constants::SHOOT_COOLDOWN;
                }
            });
    }

    inline void registerEnemyMoveSystem(flecs::world& world) {
        world.system<components::Position>("EnemyMoveSystem")
            .with<components::Enemy>()
            .kind(flecs::OnUpdate)
            .run([](flecs::iter& it) {
                auto* gs = it.world().get_mut<components::GameState>();

                gs->enemyMinX = 999.0f;
                gs->enemyMaxX = -999.0f;
                gs->activeEnemyCount = 0;

                if (gs->gameOver) {
                    while (it.next()) {}
                    return;
                }

                while (it.next()) {
                    auto pos = it.field<components::Position>(0);
                    for (auto i : it) {
                        gs->activeEnemyCount++;
                        pos[i].value.x += constants::ENEMY_SPEED * gs->enemyDirection * it.delta_time();
                        if (pos[i].value.x > gs->enemyMaxX) gs->enemyMaxX = pos[i].value.x;
                        if (pos[i].value.x < gs->enemyMinX) gs->enemyMinX = pos[i].value.x;
                    }
                }
            });
    }

    inline void registerEnemyBounceSystem(flecs::world& world) {
        world.system<components::Position>("EnemyBounceSystem")
            .with<components::Enemy>()
            .kind(flecs::PostUpdate)
            .run([](flecs::iter& it) {
                auto* gs = it.world().get_mut<components::GameState>();

                if (gs->gameOver || gs->activeEnemyCount == 0) {
                    while (it.next()) {}
                    return;
                }

                float bound = constants::WORLD_WIDTH / 2.0f - 1.2f;
                bool wallHit = gs->enemyMaxX > bound || gs->enemyMinX < -bound;

                if (!wallHit) {
                    while (it.next()) {}
                    return;
                }

                gs->enemyDirection *= -1.0f;

                while (it.next()) {
                    auto pos = it.field<components::Position>(0);
                    for (auto i : it) {
                        if (pos[i].value.x > bound) pos[i].value.x = bound;
                        if (pos[i].value.x < -bound) pos[i].value.x = -bound;
                        pos[i].value.y -= constants::ENEMY_DROP;

                        if (pos[i].value.y < -constants::WORLD_HEIGHT / 2.0f + 2.0f) {
                            gs->gameOver = true;
                            auto* game = it.world().get<components::GameRef>()->game;
                            auto* audio = it.world().get<components::AudioAssets>();
                            game->playSound(audio->death, 0.8f);
                            spdlog::info("=============================");
                            spdlog::info("       GAME OVER!");
                            spdlog::info("   Final Score: {}", gs->score);
                            spdlog::info("=============================");
                            return;
                        }
                    }
                }
            });
    }

    inline void registerEnemyWobbleSystem(flecs::world& world) {
        world.system<const components::Position, const components::MeshRef>("EnemyWobbleSystem")
            .with<components::Enemy>()
            .kind(flecs::PostUpdate)
            .each([](flecs::iter& it, size_t, const components::Position& pos, const components::MeshRef& mr) {
                auto* gs = it.world().get<components::GameState>();
                float wobble = std::sin(gs->time * 4.0f + pos.value.x * 0.5f) * 0.1f;
                if (mr.mesh) {
                    mr.mesh->setPosition({pos.value.x, pos.value.y + wobble, pos.value.z});
                }
            });
    }

    inline void registerCollisionSystem(flecs::world& world) {
        world.system("CollisionSystem")
            .kind(flecs::PostUpdate)
            .run([](flecs::iter& it) {
                while (it.next()) {}

                auto w = it.world();
                auto* gs = w.get_mut<components::GameState>();
                if (gs->gameOver) return;

                auto* game = w.get<components::GameRef>()->game;
                auto* audio = w.get<components::AudioAssets>();

                struct ProjData { flecs::entity e; glm::vec3 pos; glm::vec2 size; };
                struct EnemyData { flecs::entity e; glm::vec3 pos; glm::vec2 size; int row; };

                std::vector<ProjData> projectiles;
                std::vector<EnemyData> enemies;

                w.each([&](flecs::entity e, const components::Projectile&, const components::Position& p, const components::CollisionBox& b) {
                    projectiles.push_back({e, p.value, b.size});
                });

                w.each([&](flecs::entity e, const components::Enemy&, const components::Position& p, const components::CollisionBox& b, const components::EnemyRow& r) {
                    enemies.push_back({e, p.value, b.size, r.row});
                });

                std::vector<flecs::entity> toDestroy;

                for (auto& proj : projectiles) {
                    bool projHit = false;
                    for (auto& enemy : enemies) {
                        if (projHit) break;
                        if (std::find(toDestroy.begin(), toDestroy.end(), enemy.e) != toDestroy.end()) continue;

                        bool hit = proj.pos.x - proj.size.x * 0.5f < enemy.pos.x + enemy.size.x * 0.5f &&
                                   proj.pos.x + proj.size.x * 0.5f > enemy.pos.x - enemy.size.x * 0.5f &&
                                   proj.pos.y - proj.size.y * 0.5f < enemy.pos.y + enemy.size.y * 0.5f &&
                                   proj.pos.y + proj.size.y * 0.5f > enemy.pos.y - enemy.size.y * 0.5f;

                        if (hit) {
                            toDestroy.push_back(proj.e);
                            toDestroy.push_back(enemy.e);
                            projHit = true;

                            int points = (constants::ENEMY_ROWS - enemy.row) * 10;
                            gs->score += points;
                            game->playSound(audio->explosion, 0.6f);
                            spdlog::info("HIT! +{} pts | Total: {}", points, gs->score);
                        }
                    }
                }

                for (auto e : toDestroy) {
                    auto* mr = e.get<components::MeshRef>();
                    if (mr && mr->mesh) game->removeMesh(mr->mesh);
                    auto* ge = e.get<components::GlowEffect>();
                    if (ge && ge->glowMesh) game->removeMesh(ge->glowMesh);
                    e.destruct();
                }
            });
    }

    inline void registerProjectileCleanupSystem(flecs::world& world) {
        world.system<const components::Position>("ProjectileCleanupSystem")
            .with<components::Projectile>()
            .kind(flecs::PostUpdate)
            .run([](flecs::iter& it) {
                auto* game = it.world().get<components::GameRef>()->game;
                std::vector<flecs::entity> toDestroy;

                while (it.next()) {
                    auto pos = it.field<const components::Position>(0);
                    for (auto i : it) {
                        if (pos[i].value.y > constants::WORLD_HEIGHT / 2.0f + 1.0f) {
                            toDestroy.push_back(it.entity(i));
                        }
                    }
                }

                for (auto e : toDestroy) {
                    auto* mr = e.get<components::MeshRef>();
                    if (mr && mr->mesh) game->removeMesh(mr->mesh);
                    auto* ge = e.get<components::GlowEffect>();
                    if (ge && ge->glowMesh) game->removeMesh(ge->glowMesh);
                    e.destruct();
                }
            });
    }

    inline void registerVictorySystem(flecs::world& world) {
        world.system("VictorySystem")
            .kind(flecs::PostUpdate)
            .run([](flecs::iter& it) {
                while (it.next()) {}

                auto w = it.world();
                auto* gs = w.get_mut<components::GameState>();
                if (gs->gameOver) return;

                int count = 0;
                w.each([&](const components::Enemy&) { count++; });

                if (count == 0) {
                    gs->gameOver = true;
                    gs->victory = true;
                    auto* game = w.get<components::GameRef>()->game;
                    auto* audio = w.get<components::AudioAssets>();
                    game->playSound(audio->bonus, 0.8f);
                    spdlog::info("=============================");
                    spdlog::info("       VICTORY!");
                    spdlog::info("   Final Score: {}", gs->score);
                    spdlog::info("=============================");
                }
            });
    }

    inline void registerGlowPulseSystem(flecs::world& world) {
        world.system<const components::Position, const components::GlowEffect>("GlowPulseSystem")
            .with<components::Projectile>()
            .kind(flecs::PostUpdate)
            .each([](flecs::iter& it, size_t, const components::Position& pos, const components::GlowEffect& ge) {
                auto* gs = it.world().get<components::GameState>();
                float pulse = 1.0f + std::sin(gs->time * 15.0f) * 0.15f;
                if (ge.glowMesh) {
                    ge.glowMesh->setPosition(pos.value);
                    ge.glowMesh->setScale({pulse, pulse, 1.0f});
                }
            });
    }

    inline void registerMeshSyncSystem(flecs::world& world) {
        world.system<const components::Position, const components::MeshRef>("MeshSyncSystem")
            .without<components::Enemy>()
            .kind(flecs::PreStore)
            .each([](const components::Position& pos, const components::MeshRef& mr) {
                if (mr.mesh) {
                    mr.mesh->setPosition(pos.value);
                }
            });
    }

    inline void registerPointLightSyncSystem(flecs::world& world) {
        world.system("PointLightSyncSystem")
            .kind(flecs::PreStore)
            .run([](flecs::iter& it) {
                while (it.next()) {}

                auto w = it.world();
                auto* game = w.get<components::GameRef>()->game;
                game->clearLights();
                game->setAmbientLight({0.1f, 0.1f, 0.15f}, 1.0f);

                w.each([&](const components::Projectile&, const components::Position& p) {
                    game->addPointLight(p.value, {1.0f, 0.9f, 0.3f}, 1.5f, 2.5f);
                });
            });
    }

}
