sonicgenerations -- Android + Windows Native Renderer Edition
android/   launcher app (Gradle project) with native renderer toggle
port/      recompiled game (generated C++ + src + native renderer)

NEW: Native Rendering Translation Layer (replaces rexgpu-xenos)
- High-performance Hedgehog Engine -> Vulkan direct translation
- Based on Unleashed Recompiled and Skate3 native renderer
- 2-3x faster on mobile, 1.5-2x on desktop
- Docs: NATIVE_RENDERING.md, OPTIMIZATION_GUIDE.md, port/src/native/README.md

Easiest path: push to GitHub, Actions builds the APK for you
(.github/workflows/android.yml -> artifact "SonicGenerations-android";
tags v*.*.* also publish a GitHub Release). Step-by-step: BUILD_ANDROID.md.

Windows PC build: .github/workflows/windows.yml -> "SonicGenerations-windows"
.zip (keyboard works out of the box, now with rexgpu-sonic + rexgpu-xenos).
Details: BUILD_WINDOWS.md.

Local build: ./android_sdk.sh
  (needs JDK 17 + Android SDK: platform-35, build-tools 35.0.0,
   NDK 27.2.12479018, CMake 3.31.1), then:
  cd android && ./gradlew assembleRelease -PrexName=sonicgenerations \
    -PrexTitle="Sonic Generations" -PrexTitleId=53450848 \
    -PrexPortDir=$PWD/../port -PrexSdkDir=$PWD/sdk/rexglue-sdk

Native renderer: enabled by default, toggle via settings.txt gpu_plugin=sonic/xenos
or launcher Graphics -> Native Renderer checkbox.

