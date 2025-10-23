# MAME2003 GameCube Port

A GameCube homebrew project to port MAME2003 (MAME 0.78) to Nintendo GameCube.

This project provides a minimal, fast-loading arcade emulator optimized for the GameCube's 486 MHz Gekko CPU and limited resources.

## Project Status

**Current Phase: Foundation** - Minimal scaffold with libogc integration and build system ready for MAME2003 core integration.

## Features

- GameCube native DOL with libogc, using devkitPPC
- Docker-based reproducible build system
- GitHub Actions CI/CD for automated builds and releases
- Profile-Guided Optimization (PGO) support for performance tuning
- Build system inspired by proven GameCube homebrew projects

## Goals

- Port MAME2003 (MAME 0.78) core to GameCube
- Optimize for 486 MHz Gekko CPU and limited memory
- Start with small/8-bit arcade boards
- Keep binary size minimal and performance predictable
- Support SD Gecko/SD2SP2 for ROM loading

## Build Requirements

### Local Development
- devkitPro with devkitPPC and libogc installed
- DEVKITPRO and DEVKITPPC environment variables set

### Docker Build (Recommended)
- Docker installed and running
- 4GB+ free disk space

## Building

### Using Docker (Recommended)

```bash
# Build regular GameCube version
./docker-build.sh build gc

# Build PGO instrumented version
./docker-build.sh pgo-generate gc

# Build PGO optimized version (after collecting profile data)
./docker-build.sh pgo-optimize gc

# Clean build artifacts
./docker-build.sh clean gc
```

### Local Build

```bash
# Build GameCube DOL
make gc

# Run via psoload (requires setup)
make gc-run

# Clean
make gc-clean

# PGO builds
make gc-pgo-generate
make gc-pgo-optimize
```

## Output

Built executables are placed in `executables/`:
- `mamegc-gc.dol` - Regular GameCube build
- `mamegc-gc_pgo_gen.dol` - PGO instrumented build
- `mamegc-gc_pgo_use.dol` - PGO optimized build

## Repository Structure

```
.
├── Makefile              # Top-level build orchestration
├── Makefile.gc          # GameCube-specific build rules
├── Dockerfile           # Build environment definition
├── docker-build.sh      # Docker build script
├── docker-compose.yml   # Docker compose configuration
├── source/              # C/C++ source code
├── executables/         # Output DOL files
├── build_gc/            # Build intermediates
└── .github/workflows/   # CI/CD workflows
```

## CI/CD

GitHub Actions workflows automatically:
- Build and test on every push/PR
- Create pre-release builds from main/master branch
- Support PGO instrumented builds
- Generate downloadable artifacts

## Development Roadmap

1. **Phase 1: Foundation (Current)**
   - ✅ Platform skeleton with video/console init
   - ✅ Controller polling
   - ✅ Build system with Docker support
   - ✅ GitHub Actions CI/CD

2. **Phase 2: Core Integration**
   - Integrate MAME2003 core source
   - Minimal driver support (1-2 games proof-of-concept)
   - Basic input mapping

3. **Phase 3: Filesystem**
   - SD Gecko/SD2SP2 support
   - ROM loading from SD card
   - Save state management

4. **Phase 4: User Interface**
   - Game selector menu
   - Settings interface
   - Basic ROM browser

5. **Phase 5: Optimization**
   - Frame skip implementation
   - Audio optimization
   - PGO tuning for common games
   - Memory optimization

## Technical Notes

- Uses devkitPro devkitPPC image (20240612) with GCC 13.1.0
- libogc for GameCube hardware abstraction
- libfat for SD card filesystem access
- Profile-Guided Optimization support for performance
- Build intermediates separated by configuration (regular/PGO)
