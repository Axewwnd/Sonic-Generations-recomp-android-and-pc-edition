// Sonic Generations Hedgehog Engine - Mirage Renderer Translation
// Mirage is the graphics renderer component of Hedgehog Engine
// This file translates Mirage's D3D9 calls to our native RHI
#pragma once

#include "../rhi/rhi.h"
#include <unordered_map>
#include <memory>
#include <mutex>
#include <cstdint>

namespace sonic_native {
namespace hedgehog {

// Hedgehog Engine structures (based on UnleashedRecomp and Generations research)
// These match the Xbox 360 memory layout

enum class MirageFormat : uint32_t {
    Unknown = 0,
    A8R8G8B8 = 0x18280186,
    X8R8G8B8 = 0x28280086,
    A8B8G8R8 = 0x1A200186,
    R5G6B5 = 0x28000102,
    D24S8 = 0x2D200196,
    D24FS8 = 0x1A220197,
    G16R16F = 0x2D22AB9F,
    A16B16G16R16F = 0x1A22AB60,
    L8 = 0x28000103,        // Fixed: was duplicate of R5G6B5
    DXT1 = 0x1A200086,
    DXT2 = 0x1A200087,
    DXT3 = 0x1A20008A,      // Fixed: was duplicate of DXT2
    DXT4 = 0x1A200088,
    DXT5 = 0x1A20008B,      // Fixed: was duplicate of DXT4
};

// Guest resource types (Xbox 360 side)
struct GuestTexture {
    uint32_t refCount = 1;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 0;
    uint32_t mipLevels = 1;
    MirageFormat format = MirageFormat::Unknown;
    uint32_t usage = 0;
    void* data = nullptr;
    
    // Host side
    std::unique_ptr<RHITexture> hostTexture;
    bool dirty = true;
};

struct GuestVertexBuffer {
    uint32_t refCount = 1;
    uint32_t size = 0;
    uint32_t stride = 0;
    void* data = nullptr;
    std::unique_ptr<RHIBuffer> hostBuffer;
    bool dirty = true;
};

struct GuestIndexBuffer {
    uint32_t refCount = 1;
    uint32_t size = 0;
    bool is32Bit = false;
    void* data = nullptr;
    std::unique_ptr<RHIBuffer> hostBuffer;
    bool dirty = true;
};

struct GuestVertexDeclaration {
    struct Element {
        uint16_t stream = 0;
        uint16_t offset = 0;
        uint32_t type = 0;
        uint8_t method = 0;
        uint8_t usage = 0;
        uint8_t usageIndex = 0;
    };
    std::vector<Element> elements;
    uint64_t hash = 0;
};

struct GuestVertexShader {
    uint32_t refCount = 1;
    std::vector<uint8_t> bytecode;
    std::unique_ptr<RHIShader> hostShader;
    uint32_t constantCount = 0;
};

struct GuestPixelShader {
    uint32_t refCount = 1;
    std::vector<uint8_t> bytecode;
    std::unique_ptr<RHIShader> hostShader;
    uint32_t constantCount = 0;
};

// Mirage render state
struct MirageRenderState {
    RHIViewport viewport{};
    RHIRect scissor{};
    bool scissorEnable = false;
    
    GuestTexture* renderTarget = nullptr;
    GuestTexture* depthStencil = nullptr;
    
    GuestVertexBuffer* vertexBuffers[16] = {};
    uint32_t vertexStrides[16] = {};
    uint32_t vertexOffsets[16] = {};
    
    GuestIndexBuffer* indexBuffer = nullptr;
    
    GuestVertexDeclaration* vertexDecl = nullptr;
    GuestVertexShader* vertexShader = nullptr;
    GuestPixelShader* pixelShader = nullptr;
    
    GuestTexture* textures[16] = {};
    
    float vsConstants[256 * 4] = {};
    float psConstants[224 * 4] = {};
    
    // Pipeline state
    bool zEnable = true;
    bool zWriteEnable = true;
    RHICompareFunc zFunc = RHICompareFunc::LessEqual;
    bool alphaBlendEnable = false;
    RHIBlendFactor srcBlend = RHIBlendFactor::One;
    RHIBlendFactor destBlend = RHIBlendFactor::Zero;
    RHICullMode cullMode = RHICullMode::None;
};

// Main Mirage translator - translates Hedgehog Engine rendering to native RHI
class MirageTranslator {
public:
    static MirageTranslator& Get();
    
    bool Initialize(RHIDevice* rhiDevice);
    void Shutdown();
    
    // Resource creation (called from hooked guest functions)
    GuestTexture* CreateTexture(uint32_t width, uint32_t height, uint32_t depth, 
                                uint32_t mipLevels, MirageFormat format, uint32_t usage);
    GuestVertexBuffer* CreateVertexBuffer(uint32_t size, uint32_t usage, uint32_t fvf);
    GuestIndexBuffer* CreateIndexBuffer(uint32_t size, uint32_t usage, bool is32Bit);
    GuestVertexDeclaration* CreateVertexDeclaration(const void* elements);
    GuestVertexShader* CreateVertexShader(const void* bytecode, size_t size);
    GuestPixelShader* CreatePixelShader(const void* bytecode, size_t size);
    
    void DestroyTexture(GuestTexture* tex);
    void DestroyVertexBuffer(GuestVertexBuffer* vb);
    void DestroyIndexBuffer(GuestIndexBuffer* ib);
    void DestroyVertexDeclaration(GuestVertexDeclaration* decl);
    void DestroyVertexShader(GuestVertexShader* vs);
    void DestroyPixelShader(GuestPixelShader* ps);
    
    // Resource locking (for dynamic updates)
    void* LockTexture(GuestTexture* tex, uint32_t level, uint32_t* pitch);
    void UnlockTexture(GuestTexture* tex, uint32_t level);
    void* LockVertexBuffer(GuestVertexBuffer* vb, uint32_t offset, uint32_t size);
    void UnlockVertexBuffer(GuestVertexBuffer* vb);
    void* LockIndexBuffer(GuestIndexBuffer* ib, uint32_t offset, uint32_t size);
    void UnlockIndexBuffer(GuestIndexBuffer* ib);
    
    // Render state setting
    void SetRenderTarget(GuestTexture* rt);
    void SetDepthStencil(GuestTexture* ds);
    void SetViewport(float x, float y, float width, float height, float minZ, float maxZ);
    void SetScissorRect(int32_t left, int32_t top, int32_t right, int32_t bottom, bool enable);
    void SetStreamSource(uint32_t stream, GuestVertexBuffer* vb, uint32_t offset, uint32_t stride);
    void SetIndices(GuestIndexBuffer* ib);
    void SetVertexDeclaration(GuestVertexDeclaration* decl);
    void SetVertexShader(GuestVertexShader* vs);
    void SetPixelShader(GuestPixelShader* ps);
    void SetTexture(uint32_t sampler, GuestTexture* tex);
    void SetVertexShaderConstant(uint32_t start, const float* data, uint32_t count);
    void SetPixelShaderConstant(uint32_t start, const float* data, uint32_t count);
    void SetRenderState(uint32_t state, uint32_t value);
    void SetSamplerState(uint32_t sampler, uint32_t state, uint32_t value);
    
    // Drawing
    void DrawPrimitive(uint32_t primitiveType, uint32_t startVertex, uint32_t primitiveCount);
    void DrawIndexedPrimitive(uint32_t primitiveType, int32_t baseVertex, uint32_t minIndex, 
                              uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount);
    void Clear(uint32_t flags, uint32_t color, float z, uint32_t stencil);
    void Present();
    
    // Frame management
    void BeginFrame();
    void EndFrame();
    
    // Stats
    struct Stats {
        uint64_t drawCalls = 0;
        uint64_t textureUploads = 0;
        uint64_t bufferUploads = 0;
        uint64_t pipelineCreations = 0;
    };
    const Stats& GetStats() const { return stats_; }
    void ResetStats() { stats_ = {}; }
    
private:
    MirageTranslator() = default;
    
    RHIFormat TranslateFormat(MirageFormat fmt);
    void EnsureTextureUploaded(GuestTexture* tex);
    void EnsureVertexBufferUploaded(GuestVertexBuffer* vb);
    void EnsureIndexBufferUploaded(GuestIndexBuffer* ib);
    void UpdatePipeline();
    
    RHIDevice* rhiDevice_ = nullptr;
    RHICommandList* cmdList_ = nullptr;
    
    MirageRenderState state_;
    
    std::unordered_map<uint64_t, std::unique_ptr<RHIPipeline>> pipelineCache_;
    std::mutex pipelineMutex_;
    
    Stats stats_;
    
    // Default resources
    RHITexture* backBuffer_ = nullptr;
    RHITexture* depthBuffer_ = nullptr;
    
    bool frameBegan_ = false;
};

// Guest function hooks - these will be called from recompiled PPC code
// Addresses need to be discovered via pattern scanning or from symbols
namespace hooks {
    // These are placeholders - real addresses come from Xbox 360 XEX analysis
    // For Sonic Generations, we need to find D3D9 device methods
    
    // Example hook signatures (actual implementation will use PPC context)
    void HookCreateTexture(void* ppcContext);
    void HookCreateVertexBuffer(void* ppcContext);
    void HookCreateIndexBuffer(void* ppcContext);
    void HookCreateVertexDeclaration(void* ppcContext);
    void HookCreateVertexShader(void* ppcContext);
    void HookCreatePixelShader(void* ppcContext);
    void HookSetTexture(void* ppcContext);
    void HookDrawPrimitive(void* ppcContext);
    void HookDrawIndexedPrimitive(void* ppcContext);
    void HookPresent(void* ppcContext);
    void HookClear(void* ppcContext);
}

} // namespace hedgehog
} // namespace sonic_native
