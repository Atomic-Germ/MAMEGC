# Docker Build Setup for MAME2003 GameCube Port

This directory contains Docker-based build scripts for MAME2003 GameCube development.

## Prerequisites

- Docker installed and running
- At least 4GB free disk space for build artifacts

## Quick Start

```bash
# Build regular GameCube version
./docker-build.sh build gc

# Build PGO instrumented version
./docker-build.sh pgo-generate gc

# Build PGO optimized version (after collecting profile data)
./docker-build.sh pgo-optimize gc
```

## Available Scripts

### docker-build.sh

Main build script supporting multiple commands and platforms.

```bash
./docker-build.sh [COMMAND] [PLATFORM]

Commands:
  build          - Build regular version
  pgo-generate   - Build PGO instrumented version
  pgo-optimize   - Build PGO optimized version
  clean          - Clean build artifacts
  pgo-clean      - Clean PGO data and builds

Platforms:
  gc             - Nintendo GameCube
  all            - All supported platforms (currently just gc)

Examples:
  ./docker-build.sh build gc
  ./docker-build.sh pgo-generate gc
  ./docker-build.sh clean gc
```

## Docker Images

The build scripts use the official devkitPro Docker images:
- `devkitpro/devkitppc:20240612` - For GameCube development (GCC 13.1.0)

This specific version is chosen for optimal GameCube compatibility. GCC 15.1.0 (latest) produces larger binaries and may have GameCube regressions.

## Manual Docker Usage

```bash
# Interactive build environment
docker-compose run --rm builder

# Direct commands
docker run --rm -v "$(pwd)":/workspace -w /workspace devkitpro/devkitppc:20240612 make gc
docker run --rm -v "$(pwd)":/workspace -w /workspace devkitpro/devkitppc:20240612 make gc-pgo-generate
```

## Output Files

Built executables are placed in the `executables/` directory:
- `mamegc-gc.dol` - Regular GameCube build (~150KB)
- `mamegc-gc_pgo_gen.dol` - PGO instrumented GameCube build (~190KB)
- `mamegc-gc_pgo_use.dol` - PGO optimized GameCube build (~150KB)

## Troubleshooting

### Build Failures

- Ensure Docker has sufficient memory (4GB recommended)
- Check that all source files are present
- Try cleaning builds: `./docker-build.sh clean gc`

### PGO Issues

- Ensure `pgo_data/` directory exists with profile data before optimized builds
- Profile data must be collected on GameCube hardware or emulator
- Use `./docker-build.sh pgo-clean gc` to reset PGO data

### Permission Issues

- The scripts handle file permissions automatically
- If you encounter permission errors, ensure Docker can write to the workspace
- On Linux, you may need to run: `sudo chown -R $USER:$USER .`

## Development

To modify the build environment:
1. Edit `Dockerfile` for base image changes
2. Edit `docker-compose.yml` for volume/service configuration
3. Modify build scripts for new commands or platforms

## Performance

Typical build times on modern hardware:
- Clean build: ~30 seconds
- Incremental build: ~5 seconds
- PGO builds: ~35 seconds each

## See Also

- [BUILDING.md](../BUILDING.md) - Comprehensive build documentation
- [README.md](../README.md) - Main project documentation
- [ROADMAP.md](../ROADMAP.md) - Project roadmap and phases
