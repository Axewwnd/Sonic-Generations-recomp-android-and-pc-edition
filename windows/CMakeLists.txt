cmake_minimum_required(VERSION 3.25)
project(sonicgenerations LANGUAGES C CXX)

# Sonic Generations - Native Renderer Edition
# High-performance translation layer replacing rexgpu-xenos
# Windows build with Vulkan backend (and optional D3D12)

set(REX_PORT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../port" CACHE PATH "recompiled game dir")
set(REXSDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdk/rexglue-sdk" CACHE PATH "ReXGlue SDK tree")

if(NOT EXISTS "${REXSDK_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "ReXGlue SDK not found at ${REXSDK_DIR} - run windows_sdk.sh first")
endif()
if(NOT EXISTS "${REX_PORT_DIR}/generated/default/sources.cmake")
    message(FATAL_ERROR "no generated code at ${REX_PORT_DIR}/generated/default")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use Vulkan backend - native renderer is Vulkan-first (works on Android too)
# D3D12 can be enabled for Windows for potentially better performance on NVIDIA
set(REXGLUE_ENABLE_TRACY OFF CACHE BOOL "" FORCE)
set(REXGLUE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(REXGLUE_USE_VULKAN ON CACHE BOOL "" FORCE)
set(REXGLUE_USE_D3D12 OFF CACHE BOOL "" FORCE) # Set ON to enable D3D12 backend

if(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
    add_compile_options(-msse4.1)
endif()
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    add_compile_options(/Zc:char8_t-)
endif()

add_subdirectory("${REXSDK_DIR}" rexglue-sdk)
include("${REXSDK_DIR}/cmake/rexglue_helpers.cmake")

# --- recompiled guest code ---------------------------------------------------
set(REX_GEN_DIR "${REX_PORT_DIR}/generated/default")
include("${REX_GEN_DIR}/sources.cmake")

add_library(sonicgenerations_recomp OBJECT ${GENERATED_SOURCES})
target_include_directories(sonicgenerations_recomp PRIVATE
    "${REX_PORT_DIR}" "${REX_PORT_DIR}/src" "${REX_GEN_DIR}")
target_link_libraries(sonicgenerations_recomp PRIVATE rex::runtime)
rexglue_apply_target_settings(sonicgenerations_recomp)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(REX_RECOMP_EH /EHa)
else()
    set(REX_RECOMP_EH -fasync-exceptions)
endif()
target_precompile_headers(sonicgenerations_recomp PRIVATE
    "${REX_GEN_DIR}/sonicgenerations_pch.h")
target_compile_options(sonicgenerations_recomp PRIVATE ${REX_RECOMP_EH})

# --- Sonic Generations Native Renderer ---------------------------------------
# High-performance translation layer replacing rexgpu-xenos
# Based on Unleashed Recompiled and Skate3 native renderer

set(SONIC_NATIVE_SOURCES
    "${REX_PORT_DIR}/src/native/native_renderer.cpp"
    "${REX_PORT_DIR}/src/native/hedgehog/mirage_translator.cpp"
    "${REX_PORT_DIR}/src/native/hedgehog/shader_translator.cpp"
    "${REX_PORT_DIR}/src/native/hedgehog/shader_cache.cpp"
    "${REX_PORT_DIR}/src/native/hedgehog/shader_cache_data.cpp"
    "${REX_PORT_DIR}/src/native/thirdparty/zstd/zstddeclib.c"
    "${REX_PORT_DIR}/src/native/thirdparty/smol-v/smolv.cpp"
    "${REX_PORT_DIR}/src/native/rhi/vulkan/vulkan_rhi.cpp"
    "${REX_PORT_DIR}/src/native/rhi/null/null_rhi.h"
    "${REX_PORT_DIR}/src/native/optimizations/xenos_optimizations.cpp"
    "${REX_PORT_DIR}/src/native/gpu_plugin/sonic_gpu_plugin.cpp"
)

add_library(sonic_native STATIC ${SONIC_NATIVE_SOURCES})
target_include_directories(sonic_native PRIVATE
    "${REX_PORT_DIR}" "${REX_PORT_DIR}/src" "${REX_GEN_DIR}"
    "${REX_PORT_DIR}/src/native/thirdparty/zstd"
    "${REX_PORT_DIR}/src/native/thirdparty/smol-v"
    "${REXSDK_DIR}/include" "${REXSDK_DIR}/src")
target_link_libraries(sonic_native PUBLIC rex::runtime)
target_compile_options(sonic_native PRIVATE ${REX_RECOMP_EH} -Wno-unused-variable)
rexglue_apply_target_settings(sonic_native)

# Large shader cache file - compile with -O0 to avoid OOM
# 24MB C++ file with 6404 entries + 6.6MB compressed blob as byte array
# Needs /bigobj for MSVC (large object file)
if(MSVC)
    set_source_files_properties(
        "${REX_PORT_DIR}/src/native/hedgehog/shader_cache_data.cpp"
        PROPERTIES COMPILE_OPTIONS "/Od;/bigobj"
    )
    set_source_files_properties(
        "${REX_PORT_DIR}/src/native/thirdparty/zstd/zstddeclib.c"
        PROPERTIES COMPILE_OPTIONS "/O2"
        COMPILE_DEFINITIONS "ZSTD_DISABLE_ASM"
    )
else()
    set_source_files_properties(
        "${REX_PORT_DIR}/src/native/hedgehog/shader_cache_data.cpp"
        PROPERTIES COMPILE_FLAGS "-O0 -g0 -Wa,-mbig-obj"
    )
    set_source_files_properties(
        "${REX_PORT_DIR}/src/native/thirdparty/zstd/zstddeclib.c"
        PROPERTIES COMPILE_FLAGS "-O2 -g0 -DZSTD_DISABLE_ASM"
    )
endif()

# --- Optimized GPU plugin: rexgpu-sonic -------------------------------------
# This is our high-performance replacement for rexgpu-xenos
# Provides both native rendering and optimized Xenos fallback
add_library(rexgpu-sonic SHARED
    "${REX_PORT_DIR}/src/native/gpu_plugin/sonic_gpu_plugin.cpp"
)
target_include_directories(rexgpu-sonic PRIVATE
    "${REX_PORT_DIR}" "${REX_PORT_DIR}/src"
    "${REXSDK_DIR}/include" "${REXSDK_DIR}/src")
target_link_libraries(rexgpu-sonic PRIVATE rex::runtime rexgraphics)
target_compile_options(rexgpu-sonic PRIVATE ${REX_RECOMP_EH})
rexglue_apply_target_settings(rexgpu-sonic)
set_target_properties(rexgpu-sonic PROPERTIES OUTPUT_NAME "rexgpu-sonic")

# --- host executable ---------------------------------------------------------
add_executable(sonicgenerations "${REX_PORT_DIR}/src/main.cpp")
target_include_directories(sonicgenerations PRIVATE
    "${REX_PORT_DIR}" "${REX_PORT_DIR}/src" "${REX_GEN_DIR}")
target_compile_definitions(sonicgenerations PRIVATE
    SONIC_NATIVE_RENDERER=1
    REX_HAS_NATIVE_RENDERER=1)
target_link_libraries(sonicgenerations PRIVATE sonicgenerations_recomp rex::runtime sonic_native)
# Use BOTH plugins: sonic (native) and xenos (fallback)
# Default is sonic, fallback to xenos if native fails
rexglue_configure_target(sonicgenerations GPU_PLUGINS sonic xenos)

# --- install layout ----------------------------------------------------------
install(TARGETS sonicgenerations RUNTIME DESTINATION .)
install(FILES $<TARGET_FILE:rexruntime> DESTINATION .)
# Install both GPU plugins - native is primary, xenos is fallback
install(FILES $<TARGET_FILE:rexgpu-sonic> DESTINATION .)
install(FILES $<TARGET_FILE:rexgpu-xenos> DESTINATION .)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/sonicgenerations.toml" DESTINATION .)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/Play.bat" DESTINATION .)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../BUILD_WINDOWS.md" DESTINATION . RENAME README.md)

# Install native renderer docs
install(FILES "${REX_PORT_DIR}/src/native/README.md" DESTINATION . OPTIONAL)
