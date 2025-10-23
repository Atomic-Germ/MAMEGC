# MAME2003 GameCube Port - Project Summary

## Overview

This repository contains a complete development environment for porting MAME2003 (MAME 0.78) to the Nintendo GameCube. The project uses Docker for reproducible builds and GitHub Actions for continuous integration.

## Key Features

### Build System
- **Docker Integration**: Reproducible builds using devkitPro's official container
- **Native Support**: Can also build with local devkitPPC installation
- **PGO Support**: Profile-Guided Optimization for performance tuning
- **Cross-Platform**: Works on macOS, Linux, and Windows

### CI/CD
- **Automated Builds**: GitHub Actions builds on every push/PR
- **Automated Testing**: Build verification and output validation
- **Artifact Generation**: Downloadable DOL files from every build
- **Pre-releases**: Automatic releases from main/master branch

### Documentation
- Complete setup and usage documentation
- Development roadmap with phases
- Contributing guidelines
- Build troubleshooting guides

## Repository Structure

```
mameGC/
├── .github/
│   ├── workflows/           # CI/CD workflows
│   └── CONTRIBUTING.md      # How to contribute
├── source/                  # C/C++ source code
│   └── main.c              # Application entry point
├── executables/            # Build outputs (DOL files)
├── build_gc/               # Build intermediates
├── pgo_data/               # PGO profile data
├── Makefile                # Top-level build targets
├── Makefile.gc             # GameCube-specific build
├── Dockerfile              # Build environment
├── docker-compose.yml      # Docker configuration
├── docker-build.sh         # Build automation script
└── Documentation files     # Various .md guides
```

## Build Commands

```bash
# Regular build
./docker-build.sh build gc

# PGO instrumented (for profiling)
./docker-build.sh pgo-generate gc

# PGO optimized (after profiling)
./docker-build.sh pgo-optimize gc

# Clean
./docker-build.sh clean gc
```

## Outputs

| File | Description |
|------|-------------|
| `mamegc-gc.dol` | Standard GameCube executable |
| `mamegc-gc_pgo_gen.dol` | PGO instrumented for profiling |
| `mamegc-gc_pgo_use.dol` | PGO optimized executable |

## Technology Stack

- **Platform**: Nintendo GameCube
- **Toolchain**: devkitPPC (GCC 13.1.0)
- **SDK**: libogc
- **Storage**: libfat (SD card support)
- **Container**: Docker (devkitpro/devkitppc:20240612)
- **CI/CD**: GitHub Actions

## Current Status

**Phase 1: Foundation** ✅ COMPLETE
- Build system operational
- CI/CD configured
- Minimal application scaffold
- Documentation complete

**Next: Phase 2** - MAME2003 Core Integration

## Quick Links

- [Quick Start Guide](QUICKSTART.md)
- [Building Instructions](BUILDING.md)
- [Development Roadmap](ROADMAP.md)
- [Docker Build Info](DOCKER_BUILD_README.md)
- [Contributing Guide](.github/CONTRIBUTING.md)

## Development Workflow

1. Clone repository
2. Make changes in `source/`
3. Build with `./docker-build.sh build gc`
4. Test on GameCube hardware or emulator
5. Commit and push (CI runs automatically)
6. Open PR for review

## Performance Targets

- **Binary Size**: < 2MB (currently ~150KB)
- **Frame Rate**: 60fps for 8-bit games
- **Memory Usage**: < 16MB for core + game
- **Boot Time**: < 3 seconds to menu

## Testing

### Local Testing
```bash
./docker-build.sh build gc
# Copy executables/mamegc-gc.dol to SD card
# Run on GameCube with Swiss/loader
```

### CI Testing
- Automated on every push/PR
- Verifies builds complete successfully
- Checks output files exist
- Tests PGO builds

## Contributing

We welcome contributions! See [CONTRIBUTING.md](.github/CONTRIBUTING.md)

Areas needing help:
- MAME2003 core integration
- Driver porting
- Performance optimization
- Testing on hardware
- Documentation

## License

TODO: Add license information

## Contact

- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions
- **Pull Requests**: Always welcome!

---

**Project Status**: Build infrastructure ready, MAME2003 integration in progress

**Last Updated**: 2025-10-23
