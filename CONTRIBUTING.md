# Contributing to Carat Game Engine

Thank you for your interest in contributing to Carat!

## Getting Started

### Prerequisites

- C++23 compatible compiler (Clang 16+, GCC 13+, MSVC 2022+)
- [xmake](https://xmake.io/) build system
- SDL3
- CMake (optional, for IDE integration)

### Building

```bash
xmake config --mode=debug
xmake build
```

### Running Examples

```bash
xmake run playground_example
xmake run triangle_example
```

## How to Contribute

### Reporting Bugs

1. Check existing issues to avoid duplicates
2. Use the bug report template
3. Include a minimal reproducible example
4. Provide system information (OS, GPU, driver version)

### Suggesting Features

1. Open a feature request issue
2. Describe the use case
3. Explain the expected behavior

### Pull Requests

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Ensure code compiles without warnings
5. Test your changes
6. Commit with clear messages (`git commit -m 'Add amazing feature'`)
7. Push to your branch (`git push origin feature/amazing-feature`)
8. Open a Pull Request

## Code Style

- Use C++23 features where appropriate
- Follow existing naming conventions:
  - `PascalCase` for types and classes
  - `camelCase` for functions and variables
  - `m_` prefix for member variables
  - `UPPER_CASE` for constants
- No comments in code (self-documenting code)
- Keep functions focused and small
- Use `[[nodiscard]]` for functions returning values that shouldn't be ignored

## Project Structure

```
carat/
├── include/game_engine/    # Public headers
├── src/                    # Implementation
│   ├── core/              # Core systems
│   ├── graphics/          # Rendering (WebGPU)
│   ├── infrastructure/    # Window, input
│   └── presentation/      # Game loop
├── examples/              # Example projects
└── tests/                 # Unit tests
```

## Questions?

Open a discussion or issue if you have questions!
