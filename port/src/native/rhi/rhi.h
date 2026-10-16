// Sonic Generations Native Renderer - RHI abstraction
// Inspired by Unleashed Recompiled's Plume and Skate3's native_rhi
// Provides efficient translation layer from Hedgehog Engine D3D9 calls to modern APIs
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace sonic_native {

enum class RHIFormat : uint32_t {
    Unknown = 0,
    R8G8B8A8_UNORM,
    B8G8R8A8_UNORM,
    R8G8B8A8_SRGB,
    B5G6R5_UNORM,
    R5G6B5_UNORM,
    R16G16_FLOAT,
    R16G16B16A16_FLOAT,
    R32_FLOAT,
    R32G32_FLOAT,
    R32G32B32_FLOAT,
    R32G32B32A32_FLOAT,
    D24S8,
    D32_FLOAT,
    BC1_UNORM,
    BC2_UNORM,
    BC3_UNORM,
    BC4_UNORM,
    BC5_UNORM,
    R8_UNORM,
    R8G8_UNORM,
};

enum class RHIResourceState : uint32_t {
    Common = 0,
    RenderTarget,
    DepthWrite,
    DepthRead,
    ShaderResource,
    CopySrc,
    CopyDst,
    Present,
};

enum class RHIHeapType : uint32_t {
    Default = 0,
    Upload,
    Readback,
};

enum class RHIPrimitiveTopology : uint32_t {
    TriangleList = 0,
    TriangleStrip,
    LineList,
    PointList,
};

enum class RHIBlendFactor : uint32_t {
    Zero = 0,
    One,
    SrcColor,
    InvSrcColor,
    SrcAlpha,
    InvSrcAlpha,
    DestAlpha,
    InvDestAlpha,
    DestColor,
    InvDestColor,
};

enum class RHIBlendOp : uint32_t {
    Add = 0,
    Subtract,
    RevSubtract,
    Min,
    Max,
};

enum class RHICompareFunc : uint32_t {
    Never = 0,
    Less,
    Equal,
    LessEqual,
    Greater,
    NotEqual,
    GreaterEqual,
    Always,
};

enum class RHICullMode : uint32_t {
    None = 0,
    Front,
    Back,
};

enum class RHIFilter : uint32_t {
    Point = 0,
    Linear,
    Anisotropic,
};

enum class RHIAddressMode : uint32_t {
    Wrap = 0,
    Clamp,
    Mirror,
    Border,
};

struct RHIBufferDesc {
    uint64_t size = 0;
    RHIHeapType heap = RHIHeapType::Default;
    bool isVertexBuffer = false;
    bool isIndexBuffer = false;
    bool isConstantBuffer = false;
    bool isCopySrc = false;
    bool isCopyDst = false;
};

struct RHITextureDesc {
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t mipLevels = 1;
    uint32_t arraySize = 1;
    RHIFormat format = RHIFormat::Unknown;
    bool isRenderTarget = false;
    bool isDepthStencil = false;
    bool isCubeMap = false;
    bool is3D = false;
    uint32_t sampleCount = 1;
};

struct RHISamplerDesc {
    RHIFilter filter = RHIFilter::Linear;
    RHIAddressMode addressU = RHIAddressMode::Wrap;
    RHIAddressMode addressV = RHIAddressMode::Wrap;
    RHIAddressMode addressW = RHIAddressMode::Wrap;
    uint32_t maxAnisotropy = 1;
    float mipLODBias = 0.0f;
    float minLOD = 0.0f;
    float maxLOD = 1000.0f;
    RHICompareFunc comparisonFunc = RHICompareFunc::Always;
};

struct RHIViewport {
    float x = 0.0f, y = 0.0f;
    float width = 0.0f, height = 0.0f;
    float minDepth = 0.0f, maxDepth = 1.0f;
};

struct RHIRect {
    int32_t left = 0, top = 0, right = 0, bottom = 0;
};

struct RHIDrawArgs {
    uint32_t vertexCount = 0;
    uint32_t startVertex = 0;
    uint32_t instanceCount = 1;
};

struct RHIIndexedDrawArgs {
    uint32_t indexCount = 0;
    uint32_t startIndex = 0;
    int32_t baseVertex = 0;
    uint32_t instanceCount = 1;
};

// Forward declarations
class RHIBuffer;
class RHITexture;
class RHISampler;
class RHIShader;
class RHIPipeline;
class RHICommandList;

class RHIBuffer {
public:
    virtual ~RHIBuffer() = default;
    virtual uint64_t GetSize() const = 0;
    virtual void* Map() = 0;
    virtual void Unmap() = 0;
};

class RHITexture {
public:
    virtual ~RHITexture() = default;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual RHIFormat GetFormat() const = 0;
};

class RHISampler {
public:
    virtual ~RHISampler() = default;
};

class RHIShader {
public:
    virtual ~RHIShader() = default;
};

class RHIPipeline {
public:
    virtual ~RHIPipeline() = default;
};

class RHICommandList {
public:
    virtual ~RHICommandList() = default;
    
    virtual void SetViewport(const RHIViewport& vp) = 0;
    virtual void SetScissor(const RHIRect& rect) = 0;
    virtual void SetRenderTargets(RHITexture* color, RHITexture* depth) = 0;
    virtual void ClearColor(RHITexture* tex, const float color[4]) = 0;
    virtual void ClearDepthStencil(RHITexture* tex, float depth, uint8_t stencil) = 0;
    virtual void SetVertexBuffer(uint32_t slot, RHIBuffer* buffer, uint32_t stride, uint64_t offset) = 0;
    virtual void SetIndexBuffer(RHIBuffer* buffer, RHIFormat format, uint64_t offset) = 0;
    virtual void SetPipeline(RHIPipeline* pipeline) = 0;
    virtual void SetConstantBuffer(uint32_t slot, RHIBuffer* buffer, uint64_t offset) = 0;
    virtual void SetTexture(uint32_t slot, RHITexture* texture) = 0;
    virtual void SetSampler(uint32_t slot, RHISampler* sampler) = 0;
    virtual void Draw(const RHIDrawArgs& args) = 0;
    virtual void DrawIndexed(const RHIIndexedDrawArgs& args) = 0;
    virtual void CopyBufferToTexture(RHITexture* dst, RHIBuffer* src, uint64_t srcOffset, uint32_t mip, uint32_t slice) = 0;
    virtual void Barrier(RHITexture* tex, RHIResourceState before, RHIResourceState after) = 0;
    virtual void FlushBarriers() = 0;
};

// Main RHI device interface - similar to native_rhi but simplified for Sonic
class RHIDevice {
public:
    virtual ~RHIDevice() = default;
    
    virtual bool Initialize(void* windowHandle) = 0;
    virtual void Shutdown() = 0;
    
    virtual RHIBuffer* CreateBuffer(const RHIBufferDesc& desc) = 0;
    virtual RHITexture* CreateTexture(const RHITextureDesc& desc) = 0;
    virtual RHISampler* CreateSampler(const RHISamplerDesc& desc) = 0;
    virtual RHIShader* CreateVertexShader(const void* bytecode, size_t size) = 0;
    virtual RHIShader* CreatePixelShader(const void* bytecode, size_t size) = 0;
    virtual RHIPipeline* CreatePipeline(const struct RHIPipelineDesc& desc) = 0;
    
    virtual void DestroyBuffer(RHIBuffer* buffer) = 0;
    virtual void DestroyTexture(RHITexture* tex) = 0;
    virtual void DestroySampler(RHISampler* sampler) = 0;
    virtual void DestroyShader(RHIShader* shader) = 0;
    virtual void DestroyPipeline(RHIPipeline* pipeline) = 0;
    
    virtual RHICommandList* GetCommandList() = 0;
    virtual void Submit() = 0;
    virtual void Present() = 0;
    virtual void WaitIdle() = 0;
    
    virtual const char* GetBackendName() const = 0;
};

struct RHIBlendState {
    bool enable = false;
    RHIBlendFactor src = RHIBlendFactor::One;
    RHIBlendFactor dst = RHIBlendFactor::Zero;
    RHIBlendOp op = RHIBlendOp::Add;
    RHIBlendFactor srcAlpha = RHIBlendFactor::One;
    RHIBlendFactor dstAlpha = RHIBlendFactor::Zero;
    RHIBlendOp opAlpha = RHIBlendOp::Add;
    uint8_t writeMask = 0xF;
};

struct RHIDepthStencilState {
    bool depthEnable = true;
    bool depthWriteEnable = true;
    RHICompareFunc depthFunc = RHICompareFunc::Less;
    bool stencilEnable = false;
};

struct RHIRasterizerState {
    RHICullMode cullMode = RHICullMode::Back;
    bool frontCounterClockwise = false;
    bool depthClipEnable = true;
    bool scissorEnable = false;
    float depthBias = 0.0f;
    float slopeScaledDepthBias = 0.0f;
};

struct RHIVertexElement {
    const char* semanticName = nullptr;
    uint32_t semanticIndex = 0;
    RHIFormat format = RHIFormat::Unknown;
    uint32_t inputSlot = 0;
    uint32_t offset = 0;
};

struct RHIPipelineDesc {
    RHIShader* vs = nullptr;
    RHIShader* ps = nullptr;
    RHIVertexElement* inputElements = nullptr;
    uint32_t inputElementCount = 0;
    RHIBlendState blend;
    RHIDepthStencilState depthStencil;
    RHIRasterizerState rasterizer;
    RHIFormat rtvFormat = RHIFormat::Unknown;
    RHIFormat dsvFormat = RHIFormat::Unknown;
    RHIPrimitiveTopology topology = RHIPrimitiveTopology::TriangleList;
    uint32_t sampleCount = 1;
};

// Factory
std::unique_ptr<RHIDevice> CreateVulkanDevice();
std::unique_ptr<RHIDevice> CreateD3D12Device(); // Windows only

// Global RHI instance
RHIDevice* GetRHIDevice();
void SetRHIDevice(std::unique_ptr<RHIDevice> device);

} // namespace sonic_native
