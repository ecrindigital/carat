#pragma once

#include <memory>
#include <entt/entity/fwd.hpp>

namespace game_engine::core {

    template<typename T>
    using UniquePtr = std::unique_ptr<T>;

    using Entity = entt::entity;

    enum class Result {
        Success,
        Error
    };

} // namespace game_engine::core