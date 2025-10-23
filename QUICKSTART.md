# Quick Start Guide - MAME2003 GameCube Port

Get up and running with the MAME2003 GameCube port in minutes!

## 1. Prerequisites

Install Docker Desktop:
- **macOS/Windows:** [Download Docker Desktop](https://www.docker.com/products/docker-desktop/)
- **Linux:** Install Docker Engine via your package manager

## 2. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/mameGC.git
cd mameGC
```

## 3. Build

```bash
./docker-build.sh build gc
```

The build takes about 30 seconds. Output: `executables/mamegc-gc.dol`

## 4. Run on GameCube

### Option A: SD Card

1. Copy `mamegc-gc.dol` to your SD card
2. Use a GameCube homebrew loader (Swiss, etc.)
3. Run the DOL file

### Option B: Network (psoload)

```bash
# If you have psoload configured
make gc-run
```

## 5. Test the Application

The current build displays a simple test screen:
- Shows build information
- Tests controller input
- Press START to exit

## What's Next?

This is Phase 1 (Foundation). The MAME2003 core will be integrated in Phase 2.

See [ROADMAP.md](ROADMAP.md) for the complete development plan.

## Common Issues

**Docker not installed:**
```bash
# Install Docker first, then try again
```

**Permission denied:**
```bash
chmod +x docker-build.sh
./docker-build.sh build gc
```

**Build fails:**
```bash
# Clean and rebuild
./docker-build.sh clean gc
./docker-build.sh build gc
```

## Get Help

- Read [BUILDING.md](BUILDING.md) for detailed build instructions
- Check [DOCKER_BUILD_README.md](DOCKER_BUILD_README.md) for Docker details
- Open an issue on GitHub

## Contributing

Want to help? See [.github/CONTRIBUTING.md](.github/CONTRIBUTING.md)

---

**Current Status:** Foundation complete, ready for MAME2003 core integration!
