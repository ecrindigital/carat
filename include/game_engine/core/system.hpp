#pragma once

#include <string_view>

namespace game_engine::core {

    class System {
    public:
        virtual ~System() = default;

        virtual void update(float deltaTime) = 0;
        virtual std::string_view name() const = 0;

        virtual int priority() const { return 0; }
        virtual bool isParallelSafe() const { return true; }
    };

}
