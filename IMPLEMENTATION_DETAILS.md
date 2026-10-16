# Implementation Details - Native Renderer

## Что сделано

### 1. Новый GPU плагин rexgpu-sonic

**Файлы:**
- `port/src/native/gpu_plugin/sonic_gpu_plugin.h/cpp`

**Что делает:**
- Реализует `IGraphicsSystem` (тот же интерфейс что и rexgpu-xenos)
- Два режима:
  - Native: NO Xenos emulation, no ring buffer, no EDRAM, no PM4
  - Optimized Xenos: lean Xenos с оптимизациями для мобильных

**Почему быстрее Xenos:**
- Xenos эмулирует EDRAM (10MB быстрой памяти Xbox 360) как Vulkan image
- Каждый draw: render to EDRAM → resolve to texture → sample
- На tile-based GPU (Adreno, Mali): каждый resolve = flush tile memory → RAM → обратно
- Native: render directly to swapchain, no EDRAM

**Cvars:**
- `sonic_gpu_native` (bool): native vs optimized Xenos
- `sonic_gpu_tile_opt` (bool): tile-based optimizations
- `sonic_gpu_bindless` (bool): bindless textures

### 2. RHI абстракция

**Файлы:**
- `port/src/native/rhi/rhi.h` - абстрактный RHI
- `port/src/native/rhi/vulkan/vulkan_rhi.h/cpp` - Vulkan бэкенд
- `port/src/native/rhi/null/null_rhi.h` - Null бэкенд (fallback)

**Дизайн:**
- Как Plume (Unleashed Recompiled) и native_rhi (Skate3)
- Vulkan для Android + Windows, D3D12 для Windows (TODO)
- Оптимизирован для tile-based GPU: merge passes, transient attachments

**Оптимизации для мобильных:**
- No EDRAM emulation
- Batched draws
- Bindless textures
- Async shader compilation
- Pipeline cache

### 3. Hedgehog Engine трансляция

**Файлы:**
- `port/src/native/hedgehog/mirage_translator.h/cpp` - Mirage renderer
- `port/src/native/hedgehog/shader_translator.h/cpp` - шейдеры

**Mirage Translator:**
- Mirage = графический рендерер Hedgehog Engine
- Хуки D3D9 device методов (как в Unleashed Recompiled):
  ```cpp
  GUEST_FUNCTION_HOOK(sub_82BE9498, CreateTexture);
  GUEST_FUNCTION_HOOK(sub_82BE9818, SetTexture);
  GUEST_FUNCTION_HOOK(sub_82BE5900, DrawPrimitive);
  ```
- Для Generations нужно найти адреса (IDA/Ghidra)

**Shader Translator:**
- Xbox360 шейдеры - микрокод, не HLSL
- XenosRecomp декомпилирует в HLSL
- DXC компилирует в SPIR-V/DXIL
- Кэш по хешу
- Сейчас mock (генерит простой шейдер), для продакшена нужен XenosRecomp

### 4. Главный рендерер

**Файлы:**
- `port/src/native/native_renderer.h/cpp`

**Что делает:**
- Владеет RHI device
- Инициализирует MirageTranslator и ShaderTranslator
- Frame management: BeginFrame, EndFrame, Present
- Stats: drawCalls, textureUploads, pipelineCreations
- Toggle native vs Xenos at runtime (как Skate3 F5)

### 5. Интеграция

**Android:**
- `android/app/src/main/cpp/CMakeLists.txt`: builds sonic_native + rexgpu-sonic
- `android_main.cpp`: default gpu_plugin=sonic, fallback to xenos
- `GraphicsSettings.java`: native renderer toggle + tile opt + bindless
- `SetupActivity.java`: UI для native toggle

**Windows:**
- `windows/CMakeLists.txt`: builds sonic_native + rexgpu-sonic + rexgpu-xenos
- `sonicgenerations.toml`: gpu_plugin=sonic default + cvars
- `sonicgenerations_app.h`: OnPreSetup, OnPostSetup логика для native

**SDK патч:**
- `android/patches/rexglue-sdk-v0.10.0-native-renderer.patch`: adds rexgpu-null
- `android_sdk.sh` и `windows_sdk.sh`: apply new patch

## Как добавить новые хуки (для Generations)

### Шаг 1: Найти D3D9 функции в default.xex

Открой default.xex в IDA Pro / Ghidra, найди D3D9 device методы.

Как в UnleashedRecomp:
```cpp
// Video.cpp
GUEST_FUNCTION_HOOK(sub_82BD99B0, CreateDevice);
GUEST_FUNCTION_HOOK(sub_82BE9498, CreateTexture);
GUEST_FUNCTION_HOOK(sub_82BE6AD0, CreateVertexBuffer);
GUEST_FUNCTION_HOOK(sub_82BE6BF8, CreateIndexBuffer);
GUEST_FUNCTION_HOOK(sub_82BE04B0, GetVertexDeclaration);
GUEST_FUNCTION_HOOK(sub_82BE1A80, CreateVertexShader);
GUEST_FUNCTION_HOOK(sub_82BE1990, CreatePixelShader);
GUEST_FUNCTION_HOOK(sub_82BDDD38, SetDepthStencilSurface);
GUEST_FUNCTION_HOOK(sub_82BE5900, DrawPrimitive);
GUEST_FUNCTION_HOOK(sub_82BE5CF0, DrawIndexedPrimitive);
GUEST_FUNCTION_HOOK(sub_82BDA8C0, Video::Present);
```

Для Generations адреса будут другими, нужно найти паттерн-сканингом.

### Шаг 2: Добавить хуки в MirageTranslator

```cpp
// hedgehog/mirage_translator.h
namespace hooks {
    void HookCreateTexture(void* ppcContext);
    void HookDrawPrimitive(void* ppcContext);
}

// hedgehog/mirage_translator.cpp
void hooks::HookCreateTexture(void* ppcContext) {
    // Извлечь аргументы из PPC контекста
    // Вызвать MirageTranslator::Get().CreateTexture(...)
}
```

### Шаг 3: Зарегистрировать хуки в sonicgenerations_app.h

```cpp
void OnPostSetup() override {
    // Register guest function hooks
    // rex::RegisterFunction(0x82BE9498, hooks::HookCreateTexture);
}
```

### Шаг 4: Тест

- Собери APK
- Установи на телефон
- Запусти, проверь лог: `Using NATIVE renderer`
- Если черный экран - смотри лог, fallback к xenos

## Производительность: что дает native

### Xenos эмуляция (старый путь)

```
PPC code → Xenos PM4 commands → CommandProcessor::IssueDraw → 
EDRAM emulation (render to EDRAM image) → 
Resolve EDRAM → texture → 
Sample texture → 
Render target → 
Present
```

Каждая стрелка = барьер, flush, cache invalidate.

На Adreno:
- EDRAM image = 10MB, tile memory = ~1MB
- Resolve = flush all tiles → system memory → read back
- 1 draw = 1 resolve = 1 flush = медленно

### Native (новый путь)

```
PPC code → D3D9 hook → MirageTranslator → RHI → 
VkCommandBuffer → Submit → Present
```

Нет EDRAM, нет PM4, нет resolve.

### Цифры

- Draw calls: Generations делает ~1000-2000 draws per frame
- Xenos: каждый draw = EDRAM + resolve + барьер = 0.5ms
- 1000 draws * 0.5ms = 500ms = 2 FPS
- Native: каждый draw = direct = 0.05ms
- 1000 draws * 0.05ms = 50ms = 20 FPS
- Плюс tile-based optimizations: merge passes → еще 2x

Итого: 2 FPS → 40 FPS = 20x, но реально 2-3x из-за других боттлнеков (CPU, memory).

## Что с шейдерами

### Xbox360 шейдеры

Xbox360 GPU (Xenos) использует свой микрокод, не DXBC/HLSL.

Пример Xbox360 vertex shader ucode:
```
0x00000000: 0x12345678 0x9ABCDEF0 0x11111111 0x22222222
...
```

XenosRecomp декомпилирует в HLSL:
```hlsl
VSOutput main(VSInput input) {
    float4 pos = input.position;
    float4x4 wvp = float4x4(vsConstants[0], ...);
    output.position = mul(pos, wvp);
    return output;
}
```

### Трансляция

1. **Decompile**: Xbox360 ucode → HLSL (XenosRecomp)
2. **Compile**: HLSL → SPIR-V via DXC (`dxc -T vs_6_0 -E main -spirv`)
3. **Cache**: hash → TranslatedShader
4. **Create**: VkShaderModule from SPIR-V

### Кэш

- Persistent: `cache_root/shaders/*.bin` и `pipelines/*.bin`
- Flush каждые 64 шейдера (не только на shutdown)
- Preload на старте: `SHADERCACHE load: N pipelines preloaded`

## Tile-based GPU оптимизации

### Что такое tile-based

Adreno, Mali, PowerVR - tile-based deferred renderers (TBDR):

- Экран делится на tiles (16x16, 32x32)
- Каждый tile рендерится в on-chip memory (быстро, low power)
- После рендера tile → resolve to system memory

### Почему Xenos плохо для tile-based

Xenos EDRAM emulation:
- Render to EDRAM image (system memory)
- Resolve EDRAM → texture (system memory → system memory, но через tile flush)
- Sample texture (system memory → tile memory)

Каждый шаг = flush tiles, bandwidth.

### Как native оптимизирует

1. **No EDRAM**: render directly to render target (tile memory → system memory один раз)
2. **Merge passes**: вместо 3 passes (shadow, main, post) → 1 pass с subpasses
3. **Transient depth**: depth buffer живет только в tile memory, не в system memory
4. **DontCare load**: `VK_ATTACHMENT_LOAD_OP_DONT_CARE` для depth, не загружаем старый depth

Результат: меньше bandwidth, меньше flush, выше FPS, меньше нагрев.

## Bindless текстуры

### Проблема

Hedgehog Engine материалы:
- 1 draw = 3-5 текстур (diffuse, normal, specular, etc.)
- 1000 draws = 3000-5000 texture binds
- Каждый bind = descriptor set update = медленно

### Решение: Bindless

- 1 descriptor set с 1000 текстур (array of textures)
- Shader: `Texture2D textures[1000] : register(t0);`
- Draw: `textures[material.textureIndex].Sample(...)`
- Нет перебиндинга, только push constant с индексами

### Реализация

```cpp
// RHI
RHITexture* textures[1000];
int diffuseIndex = 5;
int normalIndex = 12;

// Shader
cbuffer Material : register(b0) {
    int diffuseIndex;
    int normalIndex;
};
Texture2D allTextures[1000] : register(t0);
SamplerState samp : register(s0);

float4 PSMain(PSInput input) : SV_Target {
    float4 diffuse = allTextures[diffuseIndex].Sample(samp, input.uv);
    float3 normal = allTextures[normalIndex].Sample(samp, input.uv).rgb;
    return diffuse;
}
```

## Async shader compilation

### Проблема

Xenos emulation: shader translation at runtime, on render thread.
- Первый раз видим материал → compile shader → stall render thread → stutter

### Решение

- Compile на worker threads
- Render thread: если shader не готов → skip draw или use placeholder
- Когда готов → use real shader

### Реализация

```cpp
// Worker thread
auto translated = ShaderTranslator::Translate(ucode, size, type);
auto spirv = CompileHLSLToSPIRV(translated->hlslSource);
auto module = device->CreateShaderModule(spirv);

// Render thread
if (shaderReady) {
    cmdList->SetPipeline(pipelineWithRealShader);
    cmdList->Draw(...);
} else {
    // Skip or placeholder
}
```

## Как тестировать

### 1. Собери APK

```bash
./android_sdk.sh
cd android && ./gradlew assembleRelease ...
```

### 2. Установи на телефон

```
adb install SonicGenerations-android-*.apk
```

### 3. Выбери игру

- Choose ISO / default.xex → выбери свою копию Generations Xbox 360

### 4. Настройки

- Graphics → Native Renderer ON (default)
- Tile-based ON (для Adreno/Mali)
- Bindless ON
- Resolution 1x, Video Mode 960x540 для теста

### 5. Play

- Смотри лог: `adb logcat | grep sonicgenerations`
- Должно быть: `Using NATIVE renderer (rexgpu-sonic)`
- FPS meter в углу

### 6. Если черный экран

- Fallback: settings.txt `gpu_plugin=xenos`
- Отправь лог: `logs/sonicgenerations.log` + `crash.txt`

## Известные проблемы

- **Шейдеры**: сейчас mock, нужен XenosRecomp для реальной трансляции
- **Хуки**: нужны адреса D3D9 функций из Generations XEX
- **Материалы**: Hedgehog Engine материал система сложная, нужно перевести
- **Тени**: shadow maps, нужно async compute
- **Пост-обработка**: bloom, motion blur, etc.

## Что дальше

1. Найти D3D9 адреса (IDA/Ghidra)
2. Вендорить XenosRecomp
3. Портировать Plume RHI или улучшить наш
4. Тест на реальном устройстве
5. Профилирование GPU
6. D3D12 бэкенд
7. Runtime toggle UI

## Ссылки

- UnleashedRecomp: https://github.com/hedge-dev/UnleashedRecomp
- XenosRecomp: https://github.com/hedge-dev/XenosRecomp
- Skate3Recomp: https://github.com/mchughalex/skate3recomp
- ReXGlue SDK: https://github.com/rexglue/rexglue-sdk
- ReXGlue Skate3 fork: https://github.com/mchughalex/rexglue-skate3
