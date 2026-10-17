// Stub shader cache for Android CI - empty cache to avoid 24MB file OOM
// Real cache is 24MB with 6404 shaders, but Android CI has limited memory
// This stub allows build to succeed, runtime will use shader translator fallback
// For production, use real shader_cache_data.cpp (24MB) with -O0

#include "shader_cache.h"

// Empty cache - no precompiled shaders, will use runtime translation
ShaderCacheEntry g_shaderCacheEntries[] = {};
const uint8_t g_compressedSpirvCache[] = {0};
const size_t g_spirvCacheCompressedSize = 0;
const size_t g_spirvCacheDecompressedSize = 0;
const size_t g_shaderCacheEntryCount = 0;
