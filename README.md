# mameGC

A GameCube homebrew project to port a limited, fast-loading subset of MAME to Nintendo GameCube.

We follow the structure and build conventions of snes9xGC where practical (targets, folders, build rules, PGO hooks), but this project does not use Snes9x. The initial commit is a scaffold to build and run a minimal libogc-powered DOL on GameCube as a base for integrating MAME cores.

## Goals
- GameCube native DOL with libogc, using devkitPPC
- Repository layout and Makefile conventions inspired by snes9xGC
- Start minimal, add MAME drivers incrementally (targeting small/8-bit boards first)
- Keep binary size small and performance predictable on 486 MHz Gekko CPU

## Build
Requirements:
- devkitPro with devkitPPC and libogc installed
- DEVKITPRO and DEVKITPPC environment variables set

Commands:
- make gc            # Build GameCube DOL (executables/mamegc-gc.dol)
- make gc-run        # Send DOL via psoload (optional, same as snes9xGC)
- make gc-clean      # Clean GameCube build
- make gc-pgo-generate / gc-pgo-optimize  # PGO hooks (future use)

## Conventions borrowed from snes9xGC
- Top-level Makefile that delegates to platform-specific Makefiles
- Makefile.gc uses devkitPPC gamecube_rules
- Output placed in executables/, intermediates in build_gc
- Optional PGO support flags and targets
- Dockerfile pinned to devkitPro devkitppc image

## Repository layout
- Makefile, Makefile.gc      Build entry points
- Dockerfile                 Dev container for reproducible builds
- source/                    C/C++ source tree (starts minimal)
- executables/               Output DOLs (created on build)

## Roadmap (high level)
1. Platform skeleton (this commit): video/console init, controller polling
2. Filesystem layer compatible with SD Gecko/SD2SP2 (reuse patterns from snes9xGC fileop)
3. Minimal MAME core bootstrap (no UI): fixed ROM set, hardcoded driver for proof-of-concept
4. Basic menu (reuse style of snes9xGC gui later), ROM selector, simple settings
5. Performance work: frameskip, audio options, blitting paths, PGO

## Notes
- This project targets GameCube first. Wii support and HBC assets are out-of-scope initially.
- We intentionally keep dependencies small; additional portlibs will be introduced only when needed for MAME integration.
