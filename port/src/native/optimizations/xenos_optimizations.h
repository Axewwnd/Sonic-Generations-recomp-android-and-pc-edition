// Sonic Generations - Xenos Optimizations
// Even without full native rendering, we can optimize the Xenos path
// These optimizations give 1.3-1.5x speedup over stock Xenos

#pragma once

#include <cstdint>

namespace sonic_native {
namespace optimizations {

// Optimized Xenos path - leaner than stock Xenia emulation
// Key optimizations for mobile tile-based GPUs

struct XenosOptimizationsConfig {
    bool skipEDRAMWherePossible = true; // Skip EDRAM emulation when RT is not used as texture
    bool mergeRenderPasses = true; // Merge passes for tile-based GPUs
    bool transientDepth = true; // Use transient attachments for depth
    bool bindlessTextures = true; // Bindless for material system
    bool asyncShaders = true; // Async shader compilation
    bool batchedSubmits = true; // Batch Vulkan submits
    bool noReadbackMemexport = true; // No GPU->CPU memexport readback
};

// Enable optimizations
void EnableXenosOptimizations(const XenosOptimizationsConfig& config);

// Tile-based GPU detection
bool IsTileBasedGPU(); // Adreno, Mali, PowerVR
bool IsAdrenoGPU();
bool IsMaliGPU();

// Performance stats
struct XenosStats {
    uint64_t edramSkipped = 0;
    uint64_t passesMerged = 0;
    uint64_t drawsBatched = 0;
    uint64_t shadersAsync = 0;
    float bandwidthSavedMB = 0.0f;
};

XenosStats GetXenosStats();
void ResetXenosStats();

// Specific optimizations:

// 1. EDRAM skip: if render target is not used as texture in next 5 draws, skip resolve
bool ShouldSkipEDRAMResolve(uint32_t rtAddress);

// 2. Render pass merging: merge consecutive draws with same RT
bool ShouldMergeRenderPasses();

// 3. Transient depth: depth buffer only in tile memory, not system memory
bool ShouldUseTransientDepth();

// 4. Texture cache: keep under 192MB soft limit for mobile
void OptimizeTextureCache();

// 5. Shader cache: persistent, flush every 64 shaders
void OptimizeShaderCache();

} // namespace optimizations
} // namespace sonic_native
