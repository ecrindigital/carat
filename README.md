# Axolotl Engine

A modern C++23 game engine built with performance and simplicity in mind.

## Features

- **WebGPU Rendering** — Cross-platform GPU abstraction via wgpu-native
- **SDL3 Windowing** — Modern window management and input handling
- **ECS Architecture** — Entity Component System powered by EnTT
- **Parallel Systems** — Multi-threaded task scheduling with Taskflow
- **Profiling** — Integrated Tracy profiler for performance analysis
- **Debug UI** — ImGui integration for runtime debugging

## Requirements

- C++23 compatible compiler (Clang 16+, GCC 13+, MSVC 2022+)
- [xmake](https://xmake.io) build system
- [just](https://github.com/casey/just) command runner
- SDL3 (via Homebrew on macOS)

## Quick Start

```bash
# Install dependencies (macOS)
brew install sdl3 just xmake

# Build and run
just run
```

## Commands

| Command | Description |
|---------|-------------|
| `just run [example]` | Build and run an example (default: triangle) |
| `just dev [example]` | Run with profiling and debug mode |
| `just test` | Run unit tests |
| `just clean` | Clean build artifacts |

## Project Structure

```
axolotl_engine/
├── include/game_engine/    # Public headers
│   ├── core/               # Types, DI container, profiling
│   ├── domain/             # ECS components and systems
│   ├── graphics/           # GPU device, buffers, pipelines
│   ├── infrastructure/     # Window, renderer, input
│   ├── presentation/       # Game loop
│   └── ui/                 # Debug UI
├── src/                    # Library implementation
├── examples/               # Example applications
│   └── triangle/           # WebGPU triangle demo
└── tests/                  # Unit tests
```

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                   Presentation                       │
│                    GameLoop                          │
├─────────────────────────────────────────────────────┤
│      Domain            │         Graphics            │
│   Components           │        GPUDevice            │
│   Systems              │        GPUBuffer            │
│                        │        GPUPipeline          │
├─────────────────────────────────────────────────────┤
│                  Infrastructure                      │
│         Window    InputManager    EcsManager         │
├─────────────────────────────────────────────────────┤
│                      Core                            │
│      DIContainer    Scheduler    Profiling           │
└─────────────────────────────────────────────────────┘
```

## Tech Stack

| Component | Library |
|-----------|---------|
| Build | xmake |
| Graphics | WebGPU (wgpu-native) |
| Windowing | SDL3 |
| ECS | EnTT |
| Math | GLM |
| Logging | spdlog |
| Profiling | Tracy |
| Debug UI | Dear ImGui |
| Parallelism | Taskflow |
| Testing | Catch2 |

## Credits

### Space Invaders Example
- **SFX**: [krial](https://opengameart.org/users/krial) - Siclone sound effects

## License

MIT
