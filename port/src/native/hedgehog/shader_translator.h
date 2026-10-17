// Sonic Generations - Xbox 360 Shader Translator
// Converts Xbox 360 shader microcode to HLSL/SPIR-V using XenosRecomp logic
// Based on hedge-dev/XenosRecomp

#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace sonic_native {
namespace hedgehog {

// Xbox 360 shader types
enum class XboxShaderType : uint32_t {
    Vertex = 0,
    Pixel = 1,
};

// Xbox 360 shader bytecode header
struct XboxShaderHeader {
    uint32_t magic; // 'S' 'H' 'D' 'R'
    uint32_t size;
    uint32_t type; // 0 = vertex, 1 = pixel
    uint32_t constantCount;
    uint32_t instructionCount;
};

// Translated shader
struct TranslatedShader {
    std::string hlslSource;
    std::vector<uint32_t> spirvBytecode; // SPIR-V for Vulkan
    std::vector<uint8_t> dxilBytecode; // DXIL for D3D12
    std::string entryPoint = "main";
    
    // Reflection
    struct ConstantBuffer {
        uint32_t registerIndex;
        uint32_t size;
        std::string name;
    };
    std::vector<ConstantBuffer> constantBuffers;
    
    struct Sampler {
        uint32_t registerIndex;
        std::string name;
    };
    std::vector<Sampler> samplers;
    
    struct Texture {
        uint32_t registerIndex;
        std::string name;
        uint32_t dimension; // 2D, 3D, Cube
    };
    std::vector<Texture> textures;
};

// Shader translator - converts Xbox 360 shaders to modern HLSL
class ShaderTranslator {
public:
    static ShaderTranslator& Get();
    
    bool Initialize();
    void Shutdown();
    
    // Translate Xbox 360 shader to HLSL/SPIR-V
    std::unique_ptr<TranslatedShader> Translate(const void* xboxBytecode, size_t size, XboxShaderType type);
    
    // Cache management
    void ClearCache();
    size_t GetCacheSize() const;
    
    // Precompile common shaders
    void PrecompileCommonShaders();
    
    // Stats
    struct Stats {
        uint64_t totalTranslations = 0;
        uint64_t cacheHits = 0;
        uint64_t cacheMisses = 0;
        uint64_t failedTranslations = 0;
    };
    const Stats& GetStats() const { return stats_; }
    
private:
    ShaderTranslator() = default;
    
    // Xenos shader decompiler logic (simplified from XenosRecomp)
    std::string DecompileVertexShader(const uint32_t* ucode, size_t dwordCount);
    std::string DecompilePixelShader(const uint32_t* ucode, size_t dwordCount);
    
    // HLSL generation helpers
    std::string GenerateHLSLHeader(XboxShaderType type);
    std::string GenerateHLSLConstants(const XboxShaderHeader& header);
    std::string GenerateHLSLSamplers(const XboxShaderHeader& header);
    
    // SPIR-V compilation via DXC
    bool CompileHLSLToSPIRV(const std::string& hlsl, XboxShaderType type, std::vector<uint32_t>& spirvOut);
    bool CompileHLSLToDXIL(const std::string& hlsl, XboxShaderType type, std::vector<uint8_t>& dxilOut);
    
    // Cache
    std::unordered_map<uint64_t, std::unique_ptr<TranslatedShader>> cache_;
    std::mutex cacheMutex_;
    
    Stats stats_;
    
    // Common shader templates for Hedgehog Engine
    static const char* s_commonVSHeader;
    static const char* s_commonPSHeader;
};

// Utility: hash shader bytecode for caching
uint64_t HashShaderBytecode(const void* data, size_t size);

// Utility: disassemble Xbox 360 shader for debugging
std::string DisassembleXboxShader(const void* bytecode, size_t size);

} // namespace hedgehog
} // namespace sonic_native
