#include <catch2/catch_all.hpp>
#include <flecs.h>
#include <game_engine/domain/component.hpp>
#include <game_engine/domain/system.hpp>

using namespace game_engine::domain;

TEST_CASE("Flecs entity creation") {
    SECTION("given world when creating entity then entity is valid") {
        flecs::world world;
        auto entity = world.entity();
        REQUIRE(entity.is_valid());
    }

    SECTION("given world when creating named entity then has name") {
        flecs::world world;
        auto entity = world.entity("test_entity");
        REQUIRE(entity.is_valid());
        REQUIRE(std::string(entity.name()) == "test_entity");
    }
}

TEST_CASE("Flecs component operations") {
    SECTION("given entity when adding component then component is readable") {
        flecs::world world;
        auto entity = world.entity();
        entity.set(components::Position{{1.0f, 2.0f, 3.0f}});

        const auto* pos = entity.get<components::Position>();
        REQUIRE(pos != nullptr);
        REQUIRE(pos->value.x == Catch::Approx(1.0f));
        REQUIRE(pos->value.y == Catch::Approx(2.0f));
        REQUIRE(pos->value.z == Catch::Approx(3.0f));
    }

    SECTION("given entity with component when removing then component is gone") {
        flecs::world world;
        auto entity = world.entity();
        entity.set(components::Position{{1.0f, 0.0f, 0.0f}});
        REQUIRE(entity.has<components::Position>());

        entity.remove<components::Position>();
        REQUIRE_FALSE(entity.has<components::Position>());
    }

    SECTION("given entity when adding multiple components then all readable") {
        flecs::world world;
        auto entity = world.entity();
        entity.set(components::Position{{1.0f, 2.0f, 3.0f}});
        entity.set(components::Velocity{{0.1f, 0.2f, 0.3f}});

        REQUIRE(entity.has<components::Position>());
        REQUIRE(entity.has<components::Velocity>());
    }
}

TEST_CASE("Flecs system execution") {
    SECTION("given system when progressing world then system runs") {
        flecs::world world;
        int callCount = 0;

        world.system("CountSystem")
            .run([&callCount](flecs::iter& it) {
                while (it.next()) {}
                callCount++;
            });

        world.progress(0.016f);
        REQUIRE(callCount == 1);

        world.progress(0.016f);
        REQUIRE(callCount == 2);
    }
}

TEST_CASE("MovementSystem integration") {
    SECTION("given entity with position and velocity when progressing then position updates") {
        flecs::world world;

        systems::registerMovementSystem(world);

        auto entity = world.entity();
        entity.set(components::Position{{0.0f, 0.0f, 0.0f}});
        entity.set(components::Velocity{{10.0f, 20.0f, 30.0f}});

        float dt = 0.1f;
        world.progress(dt);

        const auto* pos = entity.get<components::Position>();
        REQUIRE(pos != nullptr);
        REQUIRE(pos->value.x == Catch::Approx(1.0f));
        REQUIRE(pos->value.y == Catch::Approx(2.0f));
        REQUIRE(pos->value.z == Catch::Approx(3.0f));
    }

    SECTION("given entity with only position when progressing then position unchanged") {
        flecs::world world;

        systems::registerMovementSystem(world);

        auto entity = world.entity();
        entity.set(components::Position{{5.0f, 5.0f, 5.0f}});

        world.progress(0.1f);

        const auto* pos = entity.get<components::Position>();
        REQUIRE(pos != nullptr);
        REQUIRE(pos->value.x == Catch::Approx(5.0f));
        REQUIRE(pos->value.y == Catch::Approx(5.0f));
        REQUIRE(pos->value.z == Catch::Approx(5.0f));
    }
}
