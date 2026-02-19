#include <game_engine/game.hpp>
#include <spdlog/spdlog.h>

int main() {
    try {
        game_engine::Game game("Triangle Demo", 800, 600);
        game.run();
        return 0;
    } catch (const std::exception& e) {
        spdlog::error("An error occurred: {}", e.what());
        return 1;
    }
}
