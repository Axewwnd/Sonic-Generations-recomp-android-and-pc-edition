// Sonic Generations Hedgehog Engine - Mirage Translator Implementation
// High-performance native rendering path that bypasses Xenos emulation

#include "mirage_translator.h"
#include <cstring>
#include <algorithm>
#include <atomic>
#include <rex/logging.h>

namespace sonic_native {
namespace hedgehog {

MirageTranslator& MirageTranslator::Get() {
    static MirageTranslator instance;
    return instance;
}

bool MirageTranslator::Initialize(RHIDevice* rhiDevice) {
    rhiDevice_ = rhiDevice;
    if (!rhiDevice_) {
        REXLOG_ERROR("MirageTranslator: null RHI device");
        return false;
    }
    
    cmdList_ = rhiDevice_->GetCommandList();
    
    // Create backbuffer placeholders - actual backbuffer comes from presenter
    REXLOG_INFO("MirageTranslator: initialized with backend {}", rhiDevice_->GetBackendName());
    return true;
}

void MirageTranslator::Shutdown() {
    pipelineCache_.clear();
    rhiDevice_ = nullptr;
    cmdList_ = nullptr;
    REXLOG_INFO("MirageTranslator: shutdown");
}

RHIFormat MirageTranslator::TranslateFormat(MirageFormat fmt) {
    switch (fmt) {
        case MirageFormat::A8R8G8B8:
        case MirageFormat::A8B8G8R8:
            return RHIFormat::R8G8B8A8_UNORM;
        case MirageFormat::X8R8G8B8:
            return RHIFormat::B8G8R8A8_UNORM;
        case MirageFormat::R5G6B5:
            return RHIFormat::R5G6B5_UNORM;
        case MirageFormat::D24S8:
            return RHIFormat::D24S8;
        case MirageFormat::D24FS8:
            return RHIFormat::D24S8;
        case MirageFormat::G16R16F:
            return RHIFormat::R16G16_FLOAT;
        case MirageFormat::A16B16G16R16F:
            return RHIFormat::R16G16B16A16_FLOAT;
        case MirageFormat::L8:
            return RHIFormat::R8_UNORM;
        case MirageFormat::DXT1:
            return RHIFormat::BC1_UNORM;
        case MirageFormat::DXT3:
            return RHIFormat::BC2_UNORM;
        case MirageFormat::DXT5:
            return RHIFormat::BC3_UNORM;
        default:
            return RHIFormat::R8G8B8A8_UNORM;
    }
}

GuestTexture* MirageTranslator::CreateTexture(uint32_t width, uint32_t height, uint32_t depth,
                                              uint32_t mipLevels, MirageFormat format, uint32_t usage) {
    auto* tex = new GuestTexture();
    tex->width = width;
    tex->height = height;
    tex->depth = depth;
    tex->mipLevels = mipLevels;
    tex->format = format;
    
    RHITextureDesc desc;
    desc.width = width;
    desc.height = height;
    desc.depth = depth;
    desc.mipLevels = mipLevels;
    desc.format = TranslateFormat(format);
    desc.isRenderTarget = (usage & 1) != 0;
    desc.isDepthStencil = (format == MirageFormat::D24S8 || format == MirageFormat::D24FS8);
    
    if (rhiDevice_) {
        tex->hostTexture.reset(rhiDevice_->CreateTexture(desc));
    }
    
    return tex;
}

GuestVertexBuffer* MirageTranslator::CreateVertexBuffer(uint32_t size, uint32_t usage, uint32_t fvf) {
    auto* vb = new GuestVertexBuffer();
    vb->size = size;
    vb->data = malloc(size);
    
    RHIBufferDesc desc;
    desc.size = size;
    desc.heap = RHIHeapType::Default;
    desc.isVertexBuffer = true;
    desc.isCopyDst = true;
    
    if (rhiDevice_) {
        vb->hostBuffer.reset(rhiDevice_->CreateBuffer(desc));
    }
    
    return vb;
}

GuestIndexBuffer* MirageTranslator::CreateIndexBuffer(uint32_t size, uint32_t usage, bool is32Bit) {
    auto* ib = new GuestIndexBuffer();
    ib->size = size;
    ib->is32Bit = is32Bit;
    ib->data = malloc(size);
    
    RHIBufferDesc desc;
    desc.size = size;
    desc.heap = RHIHeapType::Default;
    desc.isIndexBuffer = true;
    desc.isCopyDst = true;
    
    if (rhiDevice_) {
        ib->hostBuffer.reset(rhiDevice_->CreateBuffer(desc));
    }
    
    return ib;
}

GuestVertexDeclaration* MirageTranslator::CreateVertexDeclaration(const void* elements) {
    auto* decl = new GuestVertexDeclaration();
    // Parse D3DVERTEXELEMENT9 array (terminated by 0xFF)
    const uint8_t* ptr = static_cast<const uint8_t*>(elements);
    for (int i = 0; i < 32; ++i) {
        GuestVertexDeclaration::Element elem;
        elem.stream = *reinterpret_cast<const uint16_t*>(ptr);
        ptr += 2;
        elem.offset = *reinterpret_cast<const uint16_t*>(ptr);
        ptr += 2;
        elem.type = *reinterpret_cast<const uint32_t*>(ptr);
        ptr += 4;
        elem.method = *ptr++;
        elem.usage = *ptr++;
        elem.usageIndex = *ptr++;
        ptr++; // padding
        
        if (elem.stream == 0xFF) break;
        decl->elements.push_back(elem);
    }
    
    // Hash for pipeline cache
    uint64_t hash = 1469598103934665603ULL;
    for (auto& e : decl->elements) {
        hash ^= e.stream;
        hash *= 1099511628211ULL;
        hash ^= e.offset;
        hash *= 1099511628211ULL;
        hash ^= e.type;
        hash *= 1099511628211ULL;
    }
    decl->hash = hash;
    
    return decl;
}

GuestVertexShader* MirageTranslator::CreateVertexShader(const void* bytecode, size_t size) {
    auto* vs = new GuestVertexShader();
    vs->bytecode.resize(size);
    memcpy(vs->bytecode.data(), bytecode, size);
    
    // TODO: Translate Xbox 360 shader bytecode to host shader via XenosRecomp
    // For now, create placeholder - real implementation uses XenosRecomp to convert to HLSL/SPIR-V
    if (rhiDevice_) {
        // In real implementation, we would:
        // 1. Decompile Xbox 360 shader microcode using XenosRecomp
        // 2. Convert to HLSL
        // 3. Compile to DXIL/SPIR-V
        // 4. Create host shader
        vs->hostShader.reset(rhiDevice_->CreateVertexShader(bytecode, size));
    }
    
    return vs;
}

GuestPixelShader* MirageTranslator::CreatePixelShader(const void* bytecode, size_t size) {
    auto* ps = new GuestPixelShader();
    ps->bytecode.resize(size);
    memcpy(ps->bytecode.data(), bytecode, size);
    
    if (rhiDevice_) {
        ps->hostShader.reset(rhiDevice_->CreatePixelShader(bytecode, size));
    }
    
    return ps;
}

void MirageTranslator::DestroyTexture(GuestTexture* tex) {
    if (tex) {
        if (rhiDevice_ && tex->hostTexture) {
            rhiDevice_->DestroyTexture(tex->hostTexture.release());
        }
        delete tex;
    }
}

void MirageTranslator::DestroyVertexBuffer(GuestVertexBuffer* vb) {
    if (vb) {
        if (vb->data) free(vb->data);
        if (rhiDevice_ && vb->hostBuffer) {
            rhiDevice_->DestroyBuffer(vb->hostBuffer.release());
        }
        delete vb;
    }
}

void MirageTranslator::DestroyIndexBuffer(GuestIndexBuffer* ib) {
    if (ib) {
        if (ib->data) free(ib->data);
        if (rhiDevice_ && ib->hostBuffer) {
            rhiDevice_->DestroyBuffer(ib->hostBuffer.release());
        }
        delete ib;
    }
}

void MirageTranslator::DestroyVertexDeclaration(GuestVertexDeclaration* decl) {
    delete decl;
}

void MirageTranslator::DestroyVertexShader(GuestVertexShader* vs) {
    if (vs) {
        if (rhiDevice_ && vs->hostShader) {
            rhiDevice_->DestroyShader(vs->hostShader.release());
        }
        delete vs;
    }
}

void MirageTranslator::DestroyPixelShader(GuestPixelShader* ps) {
    if (ps) {
        if (rhiDevice_ && ps->hostShader) {
            rhiDevice_->DestroyShader(ps->hostShader.release());
        }
        delete ps;
    }
}

void* MirageTranslator::LockTexture(GuestTexture* tex, uint32_t level, uint32_t* pitch) {
    if (!tex) return nullptr;
    if (pitch) *pitch = tex->width * 4; // Simplified
    return tex->data;
}

void MirageTranslator::UnlockTexture(GuestTexture* tex, uint32_t level) {
    if (tex) tex->dirty = true;
}

void* MirageTranslator::LockVertexBuffer(GuestVertexBuffer* vb, uint32_t offset, uint32_t size) {
    if (!vb || !vb->data) return nullptr;
    return static_cast<uint8_t*>(vb->data) + offset;
}

void MirageTranslator::UnlockVertexBuffer(GuestVertexBuffer* vb) {
    if (vb) vb->dirty = true;
}

void* MirageTranslator::LockIndexBuffer(GuestIndexBuffer* ib, uint32_t offset, uint32_t size) {
    if (!ib || !ib->data) return nullptr;
    return static_cast<uint8_t*>(ib->data) + offset;
}

void MirageTranslator::UnlockIndexBuffer(GuestIndexBuffer* ib) {
    if (ib) ib->dirty = true;
}

void MirageTranslator::SetRenderTarget(GuestTexture* rt) {
    state_.renderTarget = rt;
}

void MirageTranslator::SetDepthStencil(GuestTexture* ds) {
    state_.depthStencil = ds;
}

void MirageTranslator::SetViewport(float x, float y, float width, float height, float minZ, float maxZ) {
    state_.viewport.x = x;
    state_.viewport.y = y;
    state_.viewport.width = width;
    state_.viewport.height = height;
    state_.viewport.minDepth = minZ;
    state_.viewport.maxDepth = maxZ;
    
    if (cmdList_) {
        RHIViewport vp;
        vp.x = x; vp.y = y; vp.width = width; vp.height = height;
        vp.minDepth = minZ; vp.maxDepth = maxZ;
        cmdList_->SetViewport(vp);
    }
}

void MirageTranslator::SetScissorRect(int32_t left, int32_t top, int32_t right, int32_t bottom, bool enable) {
    state_.scissor.left = left;
    state_.scissor.top = top;
    state_.scissor.right = right;
    state_.scissor.bottom = bottom;
    state_.scissorEnable = enable;
    
    if (cmdList_) {
        RHIRect rect{left, top, right, bottom};
        cmdList_->SetScissor(rect);
    }
}

void MirageTranslator::SetStreamSource(uint32_t stream, GuestVertexBuffer* vb, uint32_t offset, uint32_t stride) {
    if (stream < 16) {
        state_.vertexBuffers[stream] = vb;
        state_.vertexOffsets[stream] = offset;
        state_.vertexStrides[stream] = stride;
        
        EnsureVertexBufferUploaded(vb);
        
        if (cmdList_ && vb && vb->hostBuffer) {
            cmdList_->SetVertexBuffer(stream, vb->hostBuffer.get(), stride, offset);
        }
    }
}

void MirageTranslator::SetIndices(GuestIndexBuffer* ib) {
    state_.indexBuffer = ib;
    EnsureIndexBufferUploaded(ib);
    
    if (cmdList_ && ib && ib->hostBuffer) {
        RHIFormat fmt = ib->is32Bit ? RHIFormat::R32_FLOAT : RHIFormat::R8G8B8A8_UNORM; // Simplified
        cmdList_->SetIndexBuffer(ib->hostBuffer.get(), fmt, 0);
    }
}

void MirageTranslator::SetVertexDeclaration(GuestVertexDeclaration* decl) {
    state_.vertexDecl = decl;
}

void MirageTranslator::SetVertexShader(GuestVertexShader* vs) {
    state_.vertexShader = vs;
}

void MirageTranslator::SetPixelShader(GuestPixelShader* ps) {
    state_.pixelShader = ps;
}

void MirageTranslator::SetTexture(uint32_t sampler, GuestTexture* tex) {
    if (sampler < 16) {
        state_.textures[sampler] = tex;
        EnsureTextureUploaded(tex);
        
        if (cmdList_ && tex && tex->hostTexture) {
            cmdList_->SetTexture(sampler, tex->hostTexture.get());
        }
    }
}

void MirageTranslator::SetVertexShaderConstant(uint32_t start, const float* data, uint32_t count) {
    if (start + count <= 256) {
        memcpy(&state_.vsConstants[start * 4], data, count * 4 * sizeof(float));
    }
}

void MirageTranslator::SetPixelShaderConstant(uint32_t start, const float* data, uint32_t count) {
    if (start + count <= 224) {
        memcpy(&state_.psConstants[start * 4], data, count * 4 * sizeof(float));
    }
}

void MirageTranslator::SetRenderState(uint32_t state, uint32_t value) {
    // Map D3DRS_* to our state
    switch (state) {
        case 7: // D3DRS_ZENABLE
            state_.zEnable = value != 0;
            break;
        case 8: // D3DRS_FILLMODE
            break;
        case 14: // D3DRS_ZWRITEENABLE
            state_.zWriteEnable = value != 0;
            break;
        case 15: // D3DRS_ALPHATESTENABLE
            break;
        case 19: // D3DRS_SRCBLEND
            state_.srcBlend = static_cast<RHIBlendFactor>(value);
            break;
        case 20: // D3DRS_DESTBLEND
            state_.destBlend = static_cast<RHIBlendFactor>(value);
            break;
        case 22: // D3DRS_CULLMODE
            state_.cullMode = static_cast<RHICullMode>(value);
            break;
        case 27: // D3DRS_ZFUNC
            state_.zFunc = static_cast<RHICompareFunc>(value);
            break;
        case 23: // D3DRS_ALPHABLENDENABLE
            state_.alphaBlendEnable = value != 0;
            break;
    }
}

void MirageTranslator::SetSamplerState(uint32_t sampler, uint32_t state, uint32_t value) {
    // Sampler states like filtering, addressing
}

void MirageTranslator::EnsureTextureUploaded(GuestTexture* tex) {
    if (!tex || !tex->dirty || !tex->data) return;
    
    // TODO: Upload texture data to GPU
    // This would involve copying from guest memory to host texture via staging buffer
    tex->dirty = false;
    stats_.textureUploads++;
}

void MirageTranslator::EnsureVertexBufferUploaded(GuestVertexBuffer* vb) {
    if (!vb || !vb->dirty || !vb->data) return;
    
    // Upload via staging
    vb->dirty = false;
    stats_.bufferUploads++;
}

void MirageTranslator::EnsureIndexBufferUploaded(GuestIndexBuffer* ib) {
    if (!ib || !ib->dirty || !ib->data) return;
    
    ib->dirty = false;
    stats_.bufferUploads++;
}

void MirageTranslator::UpdatePipeline() {
    if (!state_.vertexShader || !state_.pixelShader) return;
    
    // Create pipeline cache key
    uint64_t key = state_.vertexDecl ? state_.vertexDecl->hash : 0;
    key ^= reinterpret_cast<uint64_t>(state_.vertexShader);
    key ^= reinterpret_cast<uint64_t>(state_.pixelShader) << 1;
    key ^= static_cast<uint64_t>(state_.cullMode) << 32;
    key ^= static_cast<uint64_t>(state_.zFunc) << 35;
    
    std::lock_guard<std::mutex> lock(pipelineMutex_);
    auto it = pipelineCache_.find(key);
    if (it != pipelineCache_.end()) {
        if (cmdList_) {
            cmdList_->SetPipeline(it->second.get());
        }
        return;
    }
    
    // Create new pipeline
    // This is simplified - real implementation would build full pipeline desc
    // from current state
    RHIPipelineDesc desc;
    desc.vs = state_.vertexShader->hostShader.get();
    desc.ps = state_.pixelShader->hostShader.get();
    desc.rasterizer.cullMode = state_.cullMode;
    desc.depthStencil.depthEnable = state_.zEnable;
    desc.depthStencil.depthWriteEnable = state_.zWriteEnable;
    desc.depthStencil.depthFunc = state_.zFunc;
    desc.blend.enable = state_.alphaBlendEnable;
    desc.blend.src = state_.srcBlend;
    desc.blend.dst = state_.destBlend;
    
    if (rhiDevice_) {
        auto pipeline = std::unique_ptr<RHIPipeline>(rhiDevice_->CreatePipeline(desc));
        if (pipeline) {
            if (cmdList_) {
                cmdList_->SetPipeline(pipeline.get());
            }
            pipelineCache_[key] = std::move(pipeline);
            stats_.pipelineCreations++;
        }
    }
}

void MirageTranslator::DrawPrimitive(uint32_t primitiveType, uint32_t startVertex, uint32_t primitiveCount) {
    UpdatePipeline();
    
    if (cmdList_) {
        uint32_t vertexCount = primitiveCount;
        switch (primitiveType) {
            case 4: // D3DPT_TRIANGLELIST
                vertexCount = primitiveCount * 3;
                break;
            case 5: // D3DPT_TRIANGLESTRIP
                vertexCount = primitiveCount + 2;
                break;
            default:
                vertexCount = primitiveCount * 3;
                break;
        }
        
        RHIDrawArgs args;
        args.vertexCount = vertexCount;
        args.startVertex = startVertex;
        cmdList_->Draw(args);
        stats_.drawCalls++;
    }
}

void MirageTranslator::DrawIndexedPrimitive(uint32_t primitiveType, int32_t baseVertex, uint32_t minIndex,
                                            uint32_t numVertices, uint32_t startIndex, uint32_t primitiveCount) {
    UpdatePipeline();
    
    if (cmdList_) {
        uint32_t indexCount = primitiveCount;
        switch (primitiveType) {
            case 4: // D3DPT_TRIANGLELIST
                indexCount = primitiveCount * 3;
                break;
            case 5: // D3DPT_TRIANGLESTRIP
                indexCount = primitiveCount + 2;
                break;
            default:
                indexCount = primitiveCount * 3;
                break;
        }
        
        RHIIndexedDrawArgs args;
        args.indexCount = indexCount;
        args.startIndex = startIndex;
        args.baseVertex = baseVertex;
        cmdList_->DrawIndexed(args);
        stats_.drawCalls++;
    }
}

void MirageTranslator::Clear(uint32_t flags, uint32_t color, float z, uint32_t stencil) {
    if (!cmdList_) return;
    
    float colorF[4] = {
        ((color >> 16) & 0xFF) / 255.0f,
        ((color >> 8) & 0xFF) / 255.0f,
        (color & 0xFF) / 255.0f,
        ((color >> 24) & 0xFF) / 255.0f
    };
    
    if (flags & 1) { // D3DCLEAR_TARGET
        if (state_.renderTarget && state_.renderTarget->hostTexture) {
            cmdList_->ClearColor(state_.renderTarget->hostTexture.get(), colorF);
        }
    }
    if (flags & 2) { // D3DCLEAR_ZBUFFER
        if (state_.depthStencil && state_.depthStencil->hostTexture) {
            cmdList_->ClearDepthStencil(state_.depthStencil->hostTexture.get(), z, stencil);
        }
    }
}

void MirageTranslator::BeginFrame() {
    frameBegan_ = true;
    if (rhiDevice_) {
        cmdList_ = rhiDevice_->GetCommandList();
    }
}

void MirageTranslator::EndFrame() {
    frameBegan_ = false;
}

void MirageTranslator::Present() {
    if (rhiDevice_) {
        rhiDevice_->Submit();
        rhiDevice_->Present();
    }
}

} // namespace hedgehog
} // namespace sonic_native
