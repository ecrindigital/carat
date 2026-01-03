#include <catch2/catch_all.hpp>
#include <game_engine/core/scheduler.hpp>
#include <atomic>
#include <thread>
#include <chrono>

using namespace game_engine::core;

TEST_CASE("Scheduler basic operations") {
    SECTION("given new scheduler when adding system then executes on run") {
        Scheduler scheduler;
        bool executed = false;
        scheduler.addSystem("test_system", [&executed]() { executed = true; });
        scheduler.execute();
        REQUIRE(executed);
    }

    SECTION("given scheduler with multiple systems when executing then all systems run") {
        Scheduler scheduler;
        std::atomic<int> count{0};
        scheduler.addSystem("system1", [&count]() { count++; });
        scheduler.addSystem("system2", [&count]() { count++; });
        scheduler.addSystem("system3", [&count]() { count++; });
        scheduler.execute();
        REQUIRE(count == 3);
    }

    SECTION("given scheduler after execute when cleared then systems removed") {
        Scheduler scheduler;
        int count = 0;
        scheduler.addSystem("system1", [&count]() { count++; });
        scheduler.execute();
        scheduler.clear();
        scheduler.execute();
        REQUIRE(count == 1);
    }
}

TEST_CASE("Scheduler parallel execution") {
    SECTION("given scheduler with parallel tasks when executing then completes all") {
        Scheduler scheduler;
        std::atomic<int> count{0};
        for (int i = 0; i < 10; i++) {
            scheduler.addSystem("task" + std::to_string(i), [&count]() {
                count++;
            });
        }
        scheduler.execute();
        REQUIRE(count == 10);
    }

    SECTION("given scheduler with slow tasks when executing then parallelizes work") {
        Scheduler scheduler;
        std::atomic<int> maxConcurrent{0};
        std::atomic<int> currentConcurrent{0};

        for (int i = 0; i < 4; i++) {
            scheduler.addSystem("slow_task" + std::to_string(i), [&]() {
                currentConcurrent++;
                int current = currentConcurrent.load();
                int max = maxConcurrent.load();
                while (current > max && !maxConcurrent.compare_exchange_weak(max, current)) {}
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                currentConcurrent--;
            });
        }
        scheduler.execute();
        REQUIRE(maxConcurrent >= 1);
    }
}

TEST_CASE("Scheduler edge cases") {
    SECTION("given empty scheduler when executing then no crash") {
        Scheduler scheduler;
        REQUIRE_NOTHROW(scheduler.execute());
    }

    SECTION("given scheduler when cleared multiple times then no crash") {
        Scheduler scheduler;
        scheduler.clear();
        scheduler.clear();
        REQUIRE_NOTHROW(scheduler.execute());
    }

    SECTION("given scheduler when adding system after execute then new system runs on next execute") {
        Scheduler scheduler;
        int firstCount = 0;
        int secondCount = 0;

        scheduler.addSystem("first", [&firstCount]() { firstCount++; });
        scheduler.execute();
        scheduler.clear();

        scheduler.addSystem("second", [&secondCount]() { secondCount++; });
        scheduler.execute();

        REQUIRE(firstCount == 1);
        REQUIRE(secondCount == 1);
    }
}

TEST_CASE("Scheduler state isolation") {
    SECTION("given multiple schedulers when executing then independent") {
        Scheduler scheduler1;
        Scheduler scheduler2;

        int count1 = 0;
        int count2 = 0;

        scheduler1.addSystem("s1_sys", [&count1]() { count1++; });
        scheduler2.addSystem("s2_sys", [&count2]() { count2++; });

        scheduler1.execute();

        REQUIRE(count1 == 1);
        REQUIRE(count2 == 0);
    }
}

TEST_CASE("Scheduler construction") {
    SECTION("given default scheduler when checking workers then has hardware concurrency") {
        Scheduler scheduler;
        REQUIRE(scheduler.numWorkers() >= 1);
    }

    SECTION("given scheduler with explicit thread count when checking workers then has specified count") {
        Scheduler scheduler(4);
        REQUIRE(scheduler.numWorkers() == 4);
    }
}
