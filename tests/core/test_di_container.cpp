#include <catch2/catch_all.hpp>
#include <game_engine/core/di_container.hpp>
#include <string>
#include <memory>

using namespace game_engine::core;

struct IService {
    virtual ~IService() = default;
    virtual int getValue() const = 0;
};

struct ServiceA : IService {
    int value = 42;
    int getValue() const override { return value; }
};

struct ServiceB : IService {
    int getValue() const override { return 100; }
};

struct ServiceWithDependency {
    std::shared_ptr<IService> service;
    explicit ServiceWithDependency(std::shared_ptr<IService> svc) : service(std::move(svc)) {}
};

struct SimpleService {
    std::string name = "default";
};

TEST_CASE("DIContainer registration") {
    SECTION("given empty container when registering type then service is registered") {
        DIContainer container;
        container.registerType<SimpleService>();
        REQUIRE(container.isRegistered<SimpleService>());
    }

    SECTION("given empty container when registering singleton then service is registered") {
        DIContainer container;
        container.registerSingleton<SimpleService>();
        REQUIRE(container.isRegistered<SimpleService>());
    }

    SECTION("given empty container when checking unregistered service then returns false") {
        DIContainer container;
        REQUIRE_FALSE(container.isRegistered<SimpleService>());
    }

    SECTION("given container when registering with custom factory then uses factory") {
        DIContainer container;
        container.registerType<SimpleService>([](DIContainer&) {
            auto svc = std::make_shared<SimpleService>();
            svc->name = "custom";
            return svc;
        });
        auto service = container.resolve<SimpleService>();
        REQUIRE(service->name == "custom");
    }
}

TEST_CASE("DIContainer transient resolution") {
    SECTION("given type registration when resolving twice then returns different instances") {
        DIContainer container;
        container.registerType<SimpleService>();
        auto first = container.resolve<SimpleService>();
        auto second = container.resolve<SimpleService>();
        REQUIRE(first.get() != second.get());
    }

    SECTION("given type with factory when resolving then creates new instance each time") {
        DIContainer container;
        int callCount = 0;
        container.registerType<SimpleService>([&callCount](DIContainer&) {
            callCount++;
            return std::make_shared<SimpleService>();
        });
        container.resolve<SimpleService>();
        container.resolve<SimpleService>();
        REQUIRE(callCount == 2);
    }
}

TEST_CASE("DIContainer singleton resolution") {
    SECTION("given singleton registration when resolving twice then returns same instance") {
        DIContainer container;
        container.registerSingleton<SimpleService>();
        auto first = container.resolve<SimpleService>();
        auto second = container.resolve<SimpleService>();
        REQUIRE(first.get() == second.get());
    }

    SECTION("given singleton with factory when resolving twice then factory called once") {
        DIContainer container;
        int callCount = 0;
        container.registerSingleton<SimpleService>([&callCount](DIContainer&) {
            callCount++;
            return std::make_shared<SimpleService>();
        });
        container.resolve<SimpleService>();
        container.resolve<SimpleService>();
        REQUIRE(callCount == 1);
    }

    SECTION("given singleton when modifying instance then changes persist") {
        DIContainer container;
        container.registerSingleton<SimpleService>();
        auto first = container.resolve<SimpleService>();
        first->name = "modified";
        auto second = container.resolve<SimpleService>();
        REQUIRE(second->name == "modified");
    }
}

TEST_CASE("DIContainer error cases") {
    SECTION("given empty container when resolving unregistered service then throws") {
        DIContainer container;
        REQUIRE_THROWS_AS(container.resolve<SimpleService>(), std::runtime_error);
    }

    SECTION("given empty container when resolving unregistered service then error message contains type info") {
        DIContainer container;
        REQUIRE_THROWS_WITH(container.resolve<SimpleService>(), Catch::Matchers::ContainsSubstring("not registered"));
    }
}

TEST_CASE("DIContainer dependency injection") {
    SECTION("given service with dependency when resolving then injects dependency") {
        DIContainer container;
        container.registerSingleton<ServiceA>();
        container.registerType<ServiceWithDependency>([](DIContainer& c) {
            return std::make_shared<ServiceWithDependency>(c.resolve<ServiceA>());
        });
        auto service = container.resolve<ServiceWithDependency>();
        REQUIRE(service->service != nullptr);
        REQUIRE(service->service->getValue() == 42);
    }

    SECTION("given chained dependencies when resolving then resolves entire chain") {
        DIContainer container;
        container.registerSingleton<SimpleService>();
        container.registerType<ServiceA>([](DIContainer& c) {
            auto simple = c.resolve<SimpleService>();
            auto svc = std::make_shared<ServiceA>();
            svc->value = static_cast<int>(simple->name.length());
            return svc;
        });
        auto service = container.resolve<ServiceA>();
        REQUIRE(service->getValue() == 7);
    }
}

TEST_CASE("DIContainer interface registration") {
    SECTION("given interface registration when resolving then returns implementation") {
        DIContainer container;
        container.registerType<ServiceA>();
        container.registerInterface<IService, ServiceA>();
        auto service = container.resolve<IService>();
        REQUIRE(service->getValue() == 42);
    }
}

TEST_CASE("DIContainer type replacement") {
    SECTION("given type registered twice when resolving then uses last registration") {
        DIContainer container;
        container.registerType<SimpleService>([](DIContainer&) {
            auto svc = std::make_shared<SimpleService>();
            svc->name = "first";
            return svc;
        });
        container.registerType<SimpleService>([](DIContainer&) {
            auto svc = std::make_shared<SimpleService>();
            svc->name = "second";
            return svc;
        });
        auto service = container.resolve<SimpleService>();
        REQUIRE(service->name == "second");
    }
}
