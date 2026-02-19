#pragma once

#include <memory>
#include <flecs.h>

namespace game_engine::core {

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;

    using Entity = flecs::entity;

    enum class Result {
        Success,
        Error
    };

} // namespace game_engine::core
