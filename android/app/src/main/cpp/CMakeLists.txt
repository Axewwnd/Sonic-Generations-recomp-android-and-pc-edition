cmake_minimum_required(VERSION 3.25)
project(rexauto_android LANGUAGES C CXX ASM)

# Handed over from Gradle (-D... in build.gradle.kts), with fallbacks for a
# direct CMake invocation.
set(REX_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." CACHE PATH "rexauto checkout")
set(REXSDK_DIR "${REX_PROJECT_ROOT}/android/sdk/rexglue-sdk" CACHE PATH "patched ReXGlue SDK tree")
set(REX_PORT_DIR "" CACHE PATH "rexauto port dir (autoports/<name>/port)")
set(REX_APP_NAME "game" CACHE STRING "rexglue project name")

if(NOT EXISTS "${REXSDK_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "ReXGlue SDK not found at ${REXSDK_DIR} - run tools/android_sdk.sh first")
endif()
if(NOT EXISTS "${REX_PORT_DIR}/generated/default/sources.cmake")
    message(FATAL_ERROR "no generated code at ${REX_PORT_DIR}/generated/default - run the desktop pipeline (rexauto.py --until build) first")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(REXGLUE_ENABLE_TRACY OFF CACHE BOOL "" FORCE)
set(REXGLUE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(REXGLUE_USE_VULKAN ON CACHE BOOL "" FORCE)
set(REXGLUE_USE_D3D12 OFF CACHE BOOL "" FORCE)

# --- mobile performance flags --------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
  add_compile_options(-O3 -DNDEBUG -g0 -fomit-frame-pointer -fno-plt
                      -fno-semantic-interposition -fmerge-all-constants
                      -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables)
  if(NOT DEFINED ENV{REX_ANDROID_BASELINE_ARM})
    add_compile_options(-march=armv8.2-a+fp16+dotprod -mtune=cortex-a76)
  endif()
  add_link_options(-Wl,-O2 -Wl,--gc-sections -Wl,--icf=all -Wl,--as-needed -Wl,-z,now -Wl,-z,max-page-size=16384)
endif()

add_subdirectory("${REXSDK_DIR}" rexglue-sdk)

# --- libadrenotools -----------------------------------------------------------
set(ADRENOTOOLS_DIR "${REXSDK_DIR}/../libadrenotools" CACHE PATH "libadrenotools checkout")
if(EXISTS "${ADRENOTOOLS_DIR}/CMakeLists.txt")
    set(BUILD_SHARED_LIBS ON)
    add_subdirectory("${ADRENOTOOLS_DIR}" adrenotools)
    set(REX_HAVE_ADRENOTOOLS ON)
else()
    message(WARNING "libadrenotools not found at ${ADRENOTOOLS_DIR} - custom GPU drivers disabled")
endif()

# --- recompiled guest code ----------------------------------------------------
set(REX_GEN_DIR "${REX_PORT_DIR}/generated/default")
include("${REX_GEN_DIR}/sources.cmake")
set(REX_GEN_SOURCES "")
foreach(src ${GENERATED_SOURCES})
    if(IS_ABSOLUTE "${src}")
        list(APPEND REX_GEN_SOURCES "${src}")
    else()
        list(APPEND REX_GEN_SOURCES "${REX_GEN_DIR}/${src}")
    endif()
endforeach()

add_library(recomp STATIC ${REX_GEN_SOURCES})
target_include_directories(recomp PRIVATE "${REX_PORT_DIR}" "${REX_PORT_DIR}/src" "${REX_GEN_DIR}")
target_precompile_headers(recomp PRIVATE "${REX_GEN_DIR}/${REX_APP_NAME}_pch.h")
target_compile_options(recomp PRIVATE
    -g0 -O3
    -fno-rtti
    -fno-math-errno -fno-trapping-math
    -Wno-unused-variable -Wno-unused-label -Wno-unused-but-set-variable
    -Wno-unused-value -Wno-parentheses-equality)
set_target_properties(recomp PROPERTIES INTERPROCEDURAL_OPTIMIZATION OFF)
target_link_libraries(recomp PUBLIC rex::runtime rexui)

# --- Sonic Generations Native Renderer ----------------------------------------
# High-performance translation layer replacing rexgpu-xenos
# Implements Hedgehog Engine -> Vulkan direct translation
# Based on Unleashed Recompiled and Skate3 native renderer

# For Android, use REAL shader cache for 30-60 fps (6404 precompiled shaders)
# Stub cache causes 1-5 fps due to runtime shader compilation stutter
# Real cache is 24MB, 6404 shaders - eliminates compilation stutter, critical for 30-60 fps
# Compiled with -O0 to avoid OOM, LTO disabled for this target
# User reported 1-5 fps with stub, native should be 30-60 fps with real cache
set(SHADER_CACHE_DATA "${REX_PORT_DIR}/src/native/hedgehog/shader_cache_data.cpp")
message(STATUS "Using REAL shader cache for Android (6404 shaders, 24MB) - for 30-60 fps, no runtime stutter")
# Fallback to stub only if explicitly requested via env var
if(DEFINED ENV{SONIC_USE_STUB_SHADER_CACHE})
    set(SHADER_CACHE_DATA "${REX_PORT_DIR}/src/native/hedgehog/shader_cache_data_stub.cpp")
    message(STATUS "Using STUB shader cache (0 shaders) - requested via SONIC_USE_STUB_SHADER_CACHE")
endif()

set(SONIC_NATIVE_SOURCES
    "${REX_PORT_DIR}/src/native/native_renderer.cpp"
    "${REX_PORT_DIR}/src/native/hedgehog/mirage_translator.cpp"
    "${REX_PORT_DIR}/src/native/hedgehog/shader_translator.cpp"
    "${REX_PORT_DIR}/src/native/hedgehog/shader_cache.cpp"
    "${SHADER_CACHE_DATA}"
    "${REX_PORT_DIR}/src/native/thirdparty/zstd/zstddeclib.c"
    "${REX_PORT_DIR}/src/native/thirdparty/smol-v/smolv.cpp"
    "${REX_PORT_DIR}/src/native/rhi/vulkan/vulkan_rhi.cpp"
    "${REX_PORT_DIR}/src/native/optimizations/xenos_optimizations.cpp"
    "${REX_PORT_DIR}/src/native/gpu_plugin/sonic_gpu_plugin.cpp"
)

add_library(sonic_native STATIC ${SONIC_NATIVE_SOURCES})
target_include_directories(sonic_native PRIVATE
    "${REX_PORT_DIR}" "${REX_PORT_DIR}/src" "${REX_GEN_DIR}"
    "${REX_PORT_DIR}/src/native/thirdparty/zstd"
    "${REX_PORT_DIR}/src/native/thirdparty/smol-v"
    "${REXSDK_DIR}/include" "${REXSDK_DIR}/src")
target_compile_options(sonic_native PRIVATE
    -O3 -fomit-frame-pointer
    -Wno-unused-variable -Wno-unused-parameter -Wno-missing-field-initializers)
target_link_libraries(sonic_native PUBLIC rex::runtime)
set_target_properties(sonic_native PROPERTIES INTERPROCEDURAL_OPTIMIZATION OFF)

# Large shader cache file - compile with -O0 to avoid OOM, no LTO
# 24MB C++ file with 6404 entries + 6.6MB compressed blob as byte array
# Critical for 30-60 fps - eliminates runtime shader compilation stutter
set_source_files_properties(
    "${REX_PORT_DIR}/src/native/hedgehog/shader_cache_data.cpp"
    PROPERTIES 
    COMPILE_OPTIONS "-O0;-g0"
    SKIP_PRECOMPILE_HEADERS ON
)
# Also stub needs no LTO
set_source_files_properties(
    "${REX_PORT_DIR}/src/native/hedgehog/shader_cache_data_stub.cpp"
    PROPERTIES 
    COMPILE_OPTIONS "-O0;-g0"
    SKIP_PRECOMPILE_HEADERS ON
)
set_source_files_properties(
    "${REX_PORT_DIR}/src/native/thirdparty/zstd/zstddeclib.c"
    PROPERTIES 
    COMPILE_OPTIONS "-O2"
    COMPILE_DEFINITIONS "ZSTD_DISABLE_ASM"
)
# Vulkan from NDK
find_package(Vulkan QUIET)
if(Vulkan_FOUND)
    target_link_libraries(sonic_native PUBLIC Vulkan::Vulkan)
else()
    target_link_libraries(sonic_native PUBLIC vulkan)
endif()
set_target_properties(sonic_native PROPERTIES CXX_VISIBILITY_PRESET hidden)

# --- Optimized GPU plugin: rexgpu-sonic -------------------------------------
add_library(rexgpu-sonic SHARED
    "${REX_PORT_DIR}/src/native/gpu_plugin/sonic_gpu_plugin.cpp"
)
target_include_directories(rexgpu-sonic PRIVATE
    "${REX_PORT_DIR}" "${REX_PORT_DIR}/src"
    "${REXSDK_DIR}/include" "${REXSDK_DIR}/src")
target_link_libraries(rexgpu-sonic PRIVATE rex::runtime)
target_compile_options(rexgpu-sonic PRIVATE -O3 -fvisibility=hidden -Wno-unused-variable)
set_target_properties(rexgpu-sonic PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    OUTPUT_NAME "rexgpu-sonic")

# --- app library loaded by SDLActivity ----------------------------------------
add_library(main SHARED
    android_gamepad.cpp
    android_keyboard.cpp
    android_main.cpp
    "${REX_PORT_DIR}/src/main.cpp"
    "${REXSDK_DIR}/src/ui/rex_app.cpp"
)
target_include_directories(main PRIVATE
    "${REX_PORT_DIR}" "${REX_PORT_DIR}/src" "${REX_GEN_DIR}"
    "${REXSDK_DIR}/include" "${REXSDK_DIR}/src/ui")
target_compile_options(main PRIVATE -Wno-unused-variable)
target_compile_definitions(main PRIVATE
    REXGLUE_BUILD_CONFIG="$<CONFIG>"
    REX_APP_NAME="${REX_APP_NAME}"
    SONIC_NATIVE_RENDERER=1
    REX_HAS_NATIVE_RENDERER=1)
target_link_libraries(main PRIVATE recomp rex::runtime rexui sonic_native)
if(REX_HAVE_ADRENOTOOLS)
    target_compile_definitions(main PRIVATE REX_HAVE_ADRENOTOOLS=1)
    target_link_libraries(main PRIVATE adrenotools)
    add_dependencies(main main_hook file_redirect_hook gsl_alloc_hook hook_impl)
endif()
target_link_options(main PRIVATE -Wl,--export-dynamic)
add_dependencies(main rexgpu-sonic rexgpu-xenos)
set_target_properties(main PROPERTIES CXX_VISIBILITY_PRESET default VISIBILITY_INLINES_HIDDEN OFF)
