# Contributing to MAME2003 GameCube Port

Thank you for your interest in contributing to this project!

## Getting Started

1. Fork the repository
2. Clone your fork
3. Install Docker (recommended) or devkitPPC locally
4. Build the project to ensure everything works

## Development Workflow

### Local Docker Development

```bash
# Build the project
./docker-build.sh build gc

# Clean build artifacts
./docker-build.sh clean gc

# Interactive Docker environment
docker-compose run --rm builder
```

### Making Changes

1. Create a feature branch: `git checkout -b feature/your-feature`
2. Make your changes in the `source/` directory
3. Test your changes: `./docker-build.sh build gc`
4. Ensure tests pass (when available)
5. Commit with clear messages
6. Push to your fork
7. Open a Pull Request

## Code Style

- Follow existing code conventions
- Keep code compatible with GameCube constraints (limited RAM, CPU)
- Comment complex sections
- Optimize for size and performance when possible

## Build System

- Don't modify core Makefile rules without discussion
- Keep builds reproducible
- Test both regular and PGO builds when making build system changes

## Pull Request Guidelines

- Reference any related issues
- Describe what your PR does
- Include build/test results if relevant
- Keep PRs focused on a single feature/fix
- Update documentation if needed

## Testing

- Test on actual hardware when possible
- Report any build or runtime issues
- Include GameCube DOL files in testing

## Questions?

Open an issue for discussion before major changes.
