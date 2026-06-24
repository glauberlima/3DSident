# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build System

Requires devkitPro with devkitARM, libctru, citro2d, citro3d, bannertool, and makerom installed.

```bash
# Set environment (must be set before building)
export DEVKITARM=/opt/devkitpro/devkitARM
export DEVKITPRO=/opt/devkitpro

# Build (produces 3DSident.3dsx and 3DSident.cia)
make

# Clean
make clean
```

No test suite exists — this is embedded 3DS homebrew targeting real hardware/emulator.

## Architecture

Entry point is `source/main.cpp` — calls `GUI::Init()`, `GUI::MainMenu()`, `GUI::Exit()` in sequence.

**Data layer** (`include/service.h`, `source/service.cpp`): All hardware/system queries go through `Service::Get*()` functions which return plain C structs (`KernelInfo`, `SystemInfo`, `NNIDInfo`, `ConfigInfo`, `HardwareInfo`, `WifiInfo`, `StorageInfo`, `MiscInfo`, `SystemStateInfo`). These structs are populated once and passed by reference/const-ref to GUI pages.

**Presentation layer** (`source/gui.cpp`): `GUI::MainMenu()` drives a state machine via `PageState` enum. Each page is a static function (e.g., `KernelInfoPage`, `BatteryInfoPage`) that receives the pre-fetched info struct and renders using citro2d. The bottom screen shows the menu; the top screen shows page content.

**Supplemental namespaces**: `ACI` (wifi slot info), `MCUHWC` (battery temperature), `Hardware`, `System`, `Storage`, `Misc`, `Kernel`, `NNID`, `Config`, `Wifi` — each in matching `source/*.cpp` and `include/*.h` pairs.

**Rendering**: Uses citro2d/citro3d (3DS GPU libraries). Two render targets: `TARGET_TOP` (GFX_TOP) and `TARGET_BOTTOM` (GFX_BOTTOM). Text uses three C2D text buffers: static, dynamic, and size. UI colours and layout constants are defined at the top of `gui.cpp`.

**Resources**: `res/drawable/` holds `.t3s` texture sources converted to `.t3x` by `tex3ds`. Built textures go to `romfs/res/drawable/` and are accessed via romfs at runtime. `res/app.rsf`, `res/banner.png`, `res/banner.wav`, `res/logo.lz11` are CIA packaging assets.

## Key Conventions

- C++ with no RTTI, no exceptions (`-fno-rtti -fno-exceptions`), C++20 standard
- Target: ARMv6K + VFP (`-march=armv6k -mtune=mpcore -mfloat-abi=hard`)
- All modules use namespaces matching their filename
- `service.h` defines all public info structs; modules populate and return them
- `BUILD_DEBUG` macro gates `Log::Open()/Close()` calls
- Services with lifecycle (ptmu, cfgu, dsp, soc) are init'd in `GUI::Init()` and exit'd in `GUI::Exit()`; mcuHWC is init'd only on pages that need it
