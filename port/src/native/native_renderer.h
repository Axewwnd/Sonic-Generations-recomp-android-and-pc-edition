// Sonic Generations - Native Renderer Main Interface
// This is the high-level native rendering system that replaces rexgpu-xenos
// Provides optimized rendering path for Hedgehog Engine

#pragma once

#include "rhi/rhi.h"
#include "hedgehog/mirage_translator.h"
#include "hedgehog/shader_translator.h"
#include <memory>
#include <string>

namespace sonic_native {

// Configuration for native renderer
struct NativeRendererConfig {
    bool enableNativeRendering = true; // Set false to fallback to xenos
    bool enableShaderCache = true;
    bool enableAsyncShaderCompilation = true;
    bool enableBindlessTextures = true; // For modern GPUs
    bool enableTileBasedOptimizations = true; // For mobile GPUs (Adreno, Mali)
    bool enableMultiThreadedRendering = true;
    int maxAnisotropy = 4;
    int resolutionScale = 1; // 1 = native, 2 = 2x, etc.
    bool vsync = true;
    bool enableShadows = true;
    bool enableBloom = true;
    std::string shaderCachePath;
};

// Main native renderer - owns RHI and Hedgehog translation
class NativeRenderer {
public:
    static NativeRenderer& Get();
    
    bool Initialize(void* windowHandle, const NativeRendererConfig& config);
    void Shutdown();
    
    bool IsInitialized() const { return initialized_; }
    bool IsNativeRenderingEnabled() const { return config_.enableNativeRendering; }
    
    RHIDevice* GetRHIDevice() { return rhiDevice_.get(); }
    hedgehog::MirageTranslator& GetMirageTranslator() { return hedgehog::MirageTranslator::Get(); }
    hedgehog::ShaderTranslator& GetShaderTranslator() { return hedgehog::ShaderTranslator::Get(); }
    
    // Frame management
    void BeginFrame();
    void EndFrame();
    void Present();
    
    // Configuration
    void SetConfig(const NativeRendererConfig& config) { config_ = config; }
    const NativeRendererConfig& GetConfig() const { return config_; }
    
    // Stats and debugging
    struct FrameStats {
        uint64_t drawCalls = 0;
        uint64_t textureUploads = 0;
        uint64_t bufferUploads = 0;
        uint64_t pipelineCreations = 0;
        float frameTimeMs = 0.0f;
        float gpuTimeMs = 0.0f;
    };
    FrameStats GetFrameStats() const;
    void ResetStats();
    
    // Toggle between native and xenos at runtime (like Skate3)
    void SetNativeRenderingEnabled(bool enabled);
    bool CanToggleAtRuntime() const { return true; }
    
private:
    NativeRenderer() = default;
    
    bool CreateRHIDevice(void* windowHandle);
    void LogRendererInfo();
    
    std::unique_ptr<RHIDevice> rhiDevice_;
    NativeRendererConfig config_;
    bool initialized_ = false;
    
    FrameStats stats_;
    uint64_t frameCount_ = 0;
};

// Integration with ReXGlue - replaces gpu_plugin=xenos
// This provides the same interface as rexgpu-xenos but with native rendering
class SonicGPUPlugin {
public:
    static SonicGPUPlugin& Get();
    
    bool Initialize(void* windowHandle);
    void Shutdown();
    
    // Called from guest code hooks
    void OnCreateTexture(void* guestTexture);
    void OnDrawCall();
    
    bool IsUsingNativePath() const { return useNativePath_; }
    
private:
    SonicGPUPlugin() = default;
    
    bool useNativePath_ = true;
    bool initialized_ = false;
};

// Factory functions for RHI
std::unique_ptr<RHIDevice> CreateRHIDeviceForPlatform(void* windowHandle);

} // namespace sonic_native

// C interface for ReXGlue integration
extern "C" {
    // Called from PPC recompiled code to initialize native renderer
    void SonicNative_Init(void* windowHandle);
    void SonicNative_Shutdown();
    void SonicNative_BeginFrame();
    void SonicNative_EndFrame();
    void SonicNative_Present();
    bool SonicNative_IsEnabled();
}
