#pragma once

#include <memory>

#include "game_engine/core/types.hpp"

namespace game_engine::graphics {

    class Renderer {
    public:
        Renderer();

        ~Renderer();

        [[nodiscard]] core::Result initialize() const;

        void render() const;

    private:
        class Impl;
        core::UniquePtr<Impl> m_pImpl;
    };

} // namespace game_engine::graphics