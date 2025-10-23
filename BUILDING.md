# Building MAME2003 GameCube Port

This document provides detailed build instructions for the MAME2003 GameCube port.

## Prerequisites

### Option 1: Docker (Recommended)

- Docker Desktop or Docker Engine installed
- 4GB+ free disk space
- Works on macOS, Linux, and Windows

### Option 2: Native devkitPPC

- devkitPro installed ([Installation Guide](https://devkitpro.org/wiki/Getting_Started))
- devkitPPC and libogc packages installed
- Environment variables set:
  - `DEVKITPRO=/opt/devkitpro` (or your installation path)
  - `DEVKITPPC=$DEVKITPRO/devkitPPC`

## Quick Start with Docker

```bash
# Clone the repository
git clone https://github.com/YOUR_USERNAME/mameGC.git
cd mameGC

# Build GameCube DOL
./docker-build.sh build gc

# Output will be in executables/mamegc-gc.dol
```

## Build Commands

### Docker Build Script

The `docker-build.sh` script supports the following commands:

```bash
# Build regular version
./docker-build.sh build gc

# Build PGO instrumented version (for profiling)
./docker-build.sh pgo-generate gc

# Build PGO optimized version (requires profile data)
./docker-build.sh pgo-optimize gc

# Clean build artifacts
./docker-build.sh clean gc

# Clean PGO data and builds
./docker-build.sh pgo-clean gc
```

### Native Make Commands

If using devkitPPC locally:

```bash
# Build
make gc

# Clean
make gc-clean

# Run via psoload (requires setup)
make gc-run

# PGO builds
make gc-pgo-generate
make gc-pgo-optimize

# Clean PGO
make pgo-clean
```

## Build Outputs

All build outputs are placed in the `executables/` directory:

| File | Description | Size |
|------|-------------|------|
| `mamegc-gc.dol` | Regular build | ~150KB |
| `mamegc-gc_pgo_gen.dol` | PGO instrumented | ~190KB |
| `mamegc-gc_pgo_use.dol` | PGO optimized | ~150KB |

## Build Directories

- `build_gc/` - Regular build intermediates
- `build_gc_pgo_gen/` - PGO instrumented build intermediates
- `build_gc_pgo_use/` - PGO optimized build intermediates
- `pgo_data/` - PGO profile data (created during profiling)

## Profile-Guided Optimization (PGO)

PGO can improve performance by optimizing based on runtime profiles:

### Step 1: Build Instrumented Version

```bash
./docker-build.sh pgo-generate gc
```

This creates `executables/mamegc-gc_pgo_gen.dol` with profiling instrumentation.

### Step 2: Run and Collect Profile Data

1. Run `mamegc-gc_pgo_gen.dol` on GameCube hardware or emulator
2. Exercise typical use cases (play games, navigate menus)
3. Profile data will be written to `pgo_data/` directory

### Step 3: Build Optimized Version

```bash
./docker-build.sh pgo-optimize gc
```

This creates `executables/mamegc-gc_pgo_use.dol` optimized based on the profile data.

## Troubleshooting

### Docker Issues

**Build fails with permission errors:**
```bash
# Ensure Docker has write access to the directory
chmod -R 755 .
```

**Out of disk space:**
```bash
# Clean Docker images and containers
docker system prune -a
```

**Docker daemon not running:**
```bash
# Start Docker Desktop or Docker daemon
```

### Native Build Issues

**devkitPPC not found:**
```bash
# Set environment variables
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC=$DEVKITPRO/devkitPPC
```

**Missing libraries:**
```bash
# Update devkitPPC packages
sudo dkp-pacman -Syu
sudo dkp-pacman -S gamecube-dev
```

### General Build Issues

**Stale build artifacts:**
```bash
# Clean everything and rebuild
./docker-build.sh clean gc
./docker-build.sh build gc
```

**Linker errors:**
- Ensure all source files are valid C/C++
- Check for missing library dependencies in `Makefile.gc`

## Advanced Configuration

### Compiler Flags

Edit `Makefile.gc` to modify compiler flags:

```makefile
CFLAGS = -g -O3 -Wall $(MACHDEP) $(INCLUDE) $(PGO_CFLAGS)
```

### Libraries

Add libraries to the `LIBS` variable in `Makefile.gc`:

```makefile
LIBS := -lfat -logc -lm -lyourlibrary
```

### Source Directories

Add additional source directories:

```makefile
SOURCES := source source/mame source/drivers
```

## Continuous Integration

GitHub Actions automatically builds the project on every push:

- Builds are tested on `devkitpro/devkitppc:20240612` container
- Artifacts are uploaded for each successful build
- Pre-release builds created from `main`/`master` branch

## Build Performance

Typical build times:

| Build Type | Docker | Native |
|------------|--------|--------|
| Clean build | ~30s | ~10s |
| Incremental | ~5s | ~2s |
| PGO generate | ~35s | ~12s |
| PGO optimize | ~35s | ~12s |

Times vary based on system specifications and project size.
