# Axolotl Engine

# Production build & run
run:
    xmake f --enable_profiling=n -y -q && xmake -y -q && xmake run game_engine_exe

# Development mode (profiling + debug + watch)
dev:
    xmake f --enable_profiling=y -m debug -y -q && xmake -y && xmake run game_engine_exe

# Tests
test:
    xmake -y -q && xmake run tests

# Clean
clean:
    xmake clean --all
