// Null RHI - fallback when Vulkan not available
// Used for building without Vulkan SDK, or for testing

#pragma once

#include "../rhi.h"

namespace sonic_native {

class NullRHIBuffer : public RHIBuffer {
public:
    NullRHIBuffer(uint64_t size) : size_(size) {}
    uint64_t GetSize() const override { return size_; }
    void* Map() override { return nullptr; }
    void Unmap() override {}
private:
    uint64_t size_;
};

class NullRHITexture : public RHITexture {
public:
    NullRHITexture(uint32_t w, uint32_t h, RHIFormat fmt) : width_(w), height_(h), format_(fmt) {}
    uint32_t GetWidth() const override { return width_; }
    uint32_t GetHeight() const override { return height_; }
    RHIFormat GetFormat() const override { return format_; }
private:
    uint32_t width_, height_;
    RHIFormat format_;
};

class NullRHICommandList : public RHICommandList {
public:
    void SetViewport(const RHIViewport& vp) override { (void)vp; }
    void SetScissor(const RHIRect& rect) override { (void)rect; }
    void SetRenderTargets(RHITexture* color, RHITexture* depth) override { (void)color; (void)depth; }
    void ClearColor(RHITexture* tex, const float color[4]) override { (void)tex; (void)color; }
    void ClearDepthStencil(RHITexture* tex, float depth, uint8_t stencil) override { (void)tex; (void)depth; (void)stencil; }
    void SetVertexBuffer(uint32_t slot, RHIBuffer* buffer, uint32_t stride, uint64_t offset) override { (void)slot; (void)buffer; (void)stride; (void)offset; }
    void SetIndexBuffer(RHIBuffer* buffer, RHIFormat format, uint64_t offset) override { (void)buffer; (void)format; (void)offset; }
    void SetPipeline(RHIPipeline* pipeline) override { (void)pipeline; }
    void SetConstantBuffer(uint32_t slot, RHIBuffer* buffer, uint64_t offset) override { (void)slot; (void)buffer; (void)offset; }
    void SetTexture(uint32_t slot, RHITexture* texture) override { (void)slot; (void)texture; }
    void SetSampler(uint32_t slot, RHISampler* sampler) override { (void)slot; (void)sampler; }
    void Draw(const RHIDrawArgs& args) override { (void)args; }
    void DrawIndexed(const RHIIndexedDrawArgs& args) override { (void)args; }
    void CopyBufferToTexture(RHITexture* dst, RHIBuffer* src, uint64_t srcOffset, uint32_t mip, uint32_t slice) override { (void)dst; (void)src; (void)srcOffset; (void)mip; (void)slice; }
    void Barrier(RHITexture* tex, RHIResourceState before, RHIResourceState after) override { (void)tex; (void)before; (void)after; }
    void FlushBarriers() override {}
};

class NullRHIDevice : public RHIDevice {
public:
    bool Initialize(void* windowHandle) override { (void)windowHandle; return true; }
    void Shutdown() override {}
    
    RHIBuffer* CreateBuffer(const RHIBufferDesc& desc) override { return new NullRHIBuffer(desc.size); }
    RHITexture* CreateTexture(const RHITextureDesc& desc) override { return new NullRHITexture(desc.width, desc.height, desc.format); }
    RHISampler* CreateSampler(const RHISamplerDesc& desc) override { (void)desc; return nullptr; }
    RHIShader* CreateVertexShader(const void* bytecode, size_t size) override { (void)bytecode; (void)size; return nullptr; }
    RHIShader* CreatePixelShader(const void* bytecode, size_t size) override { (void)bytecode; (void)size; return nullptr; }
    RHIPipeline* CreatePipeline(const RHIPipelineDesc& desc) override { (void)desc; return nullptr; }
    
    void DestroyBuffer(RHIBuffer* buffer) override { delete buffer; }
    void DestroyTexture(RHITexture* tex) override { delete tex; }
    void DestroySampler(RHISampler* sampler) override { (void)sampler; }
    void DestroyShader(RHIShader* shader) override { (void)shader; }
    void DestroyPipeline(RHIPipeline* pipeline) override { (void)pipeline; }
    
    RHICommandList* GetCommandList() override { return &cmdList_; }
    void Submit() override {}
    void Present() override {}
    void WaitIdle() override {}
    
    const char* GetBackendName() const override { return "Null"; }
    
private:
    NullRHICommandList cmdList_;
};

} // namespace sonic_native
