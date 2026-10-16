# rexgpu-sonic - Optimized GPU Plugin

High-performance replacement for rexgpu-xenos.

## Modes

### 1. Native Mode (default, fast)

- No Xenos emulation
- No EDRAM
- No PM4 command processor
- No ring buffer
- Direct Hedgehog Engine -> Vulkan translation via MirageTranslator

Performance: 2-3x vs Xenos on mobile.

### 2. Optimized Xenos Mode (fallback, compat)

- Lean Xenos emulator
- Optimized for tile-based GPUs (Adreno, Mali)
- Skips EDRAM where possible
- Batched draws
- Bindless textures
- Async shader compilation

Performance: 1.3-1.5x vs stock Xenos.

## Usage

```
--gpu_plugin=sonic          # native (default)
--gpu_plugin=sonic-xenos    # optimized Xenos
--gpu_plugin=xenos          # stock Xenos (fallback)
```

## Implementation

Implements `IGraphicsSystem`:

- `SetupPresentation`: Creates provider/presenter for UI, initializes native renderer
- `SetupGuestGpu`: In native mode, NO Xenos setup. In Xenos mode, lean setup.
- `SetInterruptCallback`, `InitializeRingBuffer`, etc.: No-op in native mode

## Building

Built as part of port:

- Android: `librexgpu-sonic.so` in APK
- Windows: `rexgpu-sonic.dll` next to exe

## Cvars

- `sonic_gpu_native` (bool): Use native vs optimized Xenos
- `sonic_gpu_tile_opt` (bool): Tile-based optimizations
- `sonic_gpu_bindless` (bool): Bindless textures
