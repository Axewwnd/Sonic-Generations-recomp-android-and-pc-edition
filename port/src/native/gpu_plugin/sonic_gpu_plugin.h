// Sonic Generations - Optimized GPU Plugin
// This is a high-performance replacement for rexgpu-xenos
// Implements IGraphicsSystem with native rendering optimizations
// Can work in two modes:
// 1. Native mode: bypasses Xenos emulation, uses Hedgehog translation
// 2. Optimized Xenos mode: lean Xenos emulator with mobile GPU optimizations

#pragma once

#include <rex/system/interfaces/graphics.h>
#include <rex/system/gpu_plugin.h>
#include <memory>
#include <string>

namespace sonic_native {
namespace gpu {

// Optimized Xenos GPU system with native rendering support
// This replaces the stock VulkanCommandProcessor with a lean version
// that is optimized for tile-based mobile GPUs

class SonicGraphicsSystem : public rex::system::IGraphicsSystem {
public:
    SonicGraphicsSystem(bool useNativeRendering = true);
    ~SonicGraphicsSystem() override;
    
    // IGraphicsSystem interface
    rex::system::X_STATUS SetupPresentation(rex::ui::WindowedAppContext* app_context) override;
    rex::system::X_STATUS SetupGuestGpu(rex::runtime::FunctionDispatcher* function_dispatcher,
                                        rex::system::KernelState* kernel_state) override;
    bool has_presentation() const override { return hasPresentation_; }
    
    rex::ui::GraphicsProvider* provider() const override { return provider_.get(); }
    rex::ui::Presenter* presenter() const override { return presenter_.get(); }
    
    void SetInterruptCallback(uint32_t callback, uint32_t user_data) override;
    void InitializeRingBuffer(uint32_t ptr, uint32_t size_log2) override;
    void EnableReadPointerWriteBack(uint32_t ptr, uint32_t block_size_log2) override;
    void InitializeShaderStorage(const std::filesystem::path& cache_root, uint32_t title_id, bool blocking) override;
    void Shutdown() override;
    
    // Sonic-specific
    bool IsUsingNativeRendering() const { return useNativeRendering_; }
    void SetNativeRendering(bool enabled) { useNativeRendering_ = enabled; }
    
private:
    bool useNativeRendering_;
    bool hasPresentation_ = false;
    
    std::unique_ptr<rex::ui::GraphicsProvider> provider_;
    std::unique_ptr<rex::ui::Presenter> presenter_;
    
    // For Xenos fallback path
    std::unique_ptr<rex::system::IGraphicsSystem> xenosFallback_;
    
    // For native path
    void* nativeRenderer_ = nullptr;
};

// Factory function - called by ReXGlue's plugin loader
extern "C" {
    REX_GPU_PLUGIN_EXPORT uint32_t rex_gpu_abi_version(void);
    REX_GPU_PLUGIN_EXPORT rex::system::IGraphicsSystem* rex_gpu_create(
        uint32_t abi_version, const rex::system::GpuCreateInfo* info);
}

// Optimized command processor for mobile GPUs
// Key optimizations:
// - No EDRAM emulation (render directly to swapchain)
// - Batched draw calls
// - Bindless textures
// - Async shader compilation
// - Tile-based rendering optimizations
class OptimizedVulkanCommandProcessor {
public:
    // This would be a full implementation of a lean command processor
    // that is much faster than stock Xenos emulation
    // For now, it's a placeholder that shows the architecture
    
    struct Stats {
        uint64_t drawsBatched = 0;
        uint64_t edramAvoided = 0;
        uint64_t shadersAsyncCompiled = 0;
    };
    
    static void EnableTileBasedOptimizations(bool enable);
    static void EnableBindlessTextures(bool enable);
    static void EnableAsyncShaderCompilation(bool enable);
};

} // namespace gpu
} // namespace sonic_native
