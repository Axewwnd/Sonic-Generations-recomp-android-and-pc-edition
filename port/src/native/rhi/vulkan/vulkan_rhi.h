// Sonic Generations Native Renderer - Vulkan RHI
// High-performance Vulkan backend optimized for mobile tile-based GPUs

#pragma once

#include "../rhi.h"

#if defined(__has_include)
#if __has_include(<vulkan/vulkan.h>)
#include <vulkan/vulkan.h>
#define SONIC_HAS_VULKAN 1
#else
#define SONIC_HAS_VULKAN 0
// Stub Vulkan types if header not available
typedef struct VkDevice_T* VkDevice;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkInstance_T* VkInstance;
typedef struct VkQueue_T* VkQueue;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
typedef struct VkSwapchainKHR_T* VkSwapchainKHR;
typedef struct VkImage_T* VkImage;
typedef struct VkImageView_T* VkImageView;
typedef struct VkBuffer_T* VkBuffer;
typedef struct VkDeviceMemory_T* VkDeviceMemory;
typedef struct VkSampler_T* VkSampler;
typedef struct VkShaderModule_T* VkShaderModule;
typedef struct VkPipeline_T* VkPipeline;
typedef struct VkPipelineLayout_T* VkPipelineLayout;
typedef struct VkCommandPool_T* VkCommandPool;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkSemaphore_T* VkSemaphore;
typedef struct VkFence_T* VkFence;
#define VK_NULL_HANDLE nullptr
typedef int VkResult;
typedef int VkFormat;
typedef int VkFilter;
typedef int VkSamplerAddressMode;
typedef int VkIndexType;
typedef int VkImageLayout;
typedef int VkAccessFlags;
typedef int VkPipelineStageFlags;
typedef int VkSampleCountFlagBits;
typedef int VkImageType;
typedef int VkImageTiling;
typedef int VkImageUsageFlags;
typedef int VkSharingMode;
typedef int VkImageViewType;
typedef int VkBorderColor;
typedef int VkCompareOp;
typedef int VkSamplerMipmapMode;
typedef int VkPipelineBindPoint;
typedef int VkCommandBufferUsageFlags;
typedef int VkCommandPoolCreateFlags;
typedef int VkMemoryPropertyFlags;
typedef uint32_t VkFlags;
typedef uint64_t VkDeviceSize;
struct VkApplicationInfo {};
struct VkInstanceCreateInfo {};
struct VkDeviceQueueCreateInfo {};
struct VkPhysicalDeviceFeatures {};
struct VkDeviceCreateInfo {};
struct VkCommandPoolCreateInfo {};
struct VkCommandBufferAllocateInfo {};
struct VkMemoryAllocateInfo {};
struct VkBufferCreateInfo {};
struct VkImageCreateInfo {};
struct VkImageViewCreateInfo {};
struct VkSamplerCreateInfo {};
struct VkShaderModuleCreateInfo {};
struct VkPipelineLayoutCreateInfo {};
struct VkViewport {};
struct VkRect2D {};
struct VkClearColorValue { float float32[4]; };
struct VkClearDepthStencilValue { float depth; uint32_t stencil; };
struct VkImageSubresourceRange {};
struct VkBufferImageCopy {};
struct VkImageMemoryBarrier {};
struct VkCommandBufferBeginInfo {};
struct VkSemaphoreCreateInfo {};
struct VkFenceCreateInfo {};
struct VkSubmitInfo {};
struct VkMemoryRequirements {};
struct VkPhysicalDeviceMemoryProperties { uint32_t memoryTypeCount; struct { VkFlags propertyFlags; } memoryTypes[32]; };
struct VkQueueFamilyProperties { VkFlags queueFlags; };
struct VkPhysicalDeviceProperties { int deviceType; };
#define VK_STRUCTURE_TYPE_APPLICATION_INFO 0
#define VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO 0
#define VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO 0
#define VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO 0
#define VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_FENCE_CREATE_INFO 0
#define VK_STRUCTURE_TYPE_SUBMIT_INFO 0
#define VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER 0
#define VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT 0
#define VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT 0
#define VK_QUEUE_GRAPHICS_BIT 1
#define VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 2
#define VK_FORMAT_R8G8B8A8_UNORM 0
#define VK_FORMAT_B8G8R8A8_UNORM 0
#define VK_FORMAT_R5G6B5_UNORM_PACK16 0
#define VK_FORMAT_D24_UNORM_S8_UINT 0
#define VK_FORMAT_D32_SFLOAT 0
#define VK_FORMAT_BC1_RGBA_UNORM_BLOCK 0
#define VK_FORMAT_BC2_UNORM_BLOCK 0
#define VK_FORMAT_BC3_UNORM_BLOCK 0
#define VK_FORMAT_R8_UNORM 0
#define VK_FORMAT_R8G8_UNORM 0
#define VK_FORMAT_R16G16_SFLOAT 0
#define VK_FORMAT_R16G16B16A16_SFLOAT 0
#define VK_FORMAT_R32_SFLOAT 0
#define VK_FILTER_NEAREST 0
#define VK_FILTER_LINEAR 0
#define VK_SAMPLER_ADDRESS_MODE_REPEAT 0
#define VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE 0
#define VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT 0
#define VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER 0
#define VK_INDEX_TYPE_UINT32 0
#define VK_INDEX_TYPE_UINT16 0
#define VK_IMAGE_LAYOUT_GENERAL 0
#define VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL 0
#define VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL 0
#define VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL 0
#define VK_IMAGE_LAYOUT_UNDEFINED 0
#define VK_IMAGE_ASPECT_COLOR_BIT 1
#define VK_IMAGE_ASPECT_DEPTH_BIT 2
#define VK_IMAGE_ASPECT_STENCIL_BIT 4
#define VK_ACCESS_TRANSFER_WRITE_BIT 0
#define VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT 0
#define VK_ACCESS_SHADER_READ_BIT 0
#define VK_PIPELINE_STAGE_ALL_COMMANDS_BIT 0
#define VK_PIPELINE_BIND_POINT_GRAPHICS 0
#define VK_IMAGE_TYPE_2D 0
#define VK_IMAGE_TYPE_3D 0
#define VK_IMAGE_TILING_OPTIMAL 0
#define VK_IMAGE_USAGE_SAMPLED_BIT 0
#define VK_IMAGE_USAGE_TRANSFER_DST_BIT 0
#define VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT 0
#define VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT 0
#define VK_SHARING_MODE_EXCLUSIVE 0
#define VK_IMAGE_VIEW_TYPE_2D 0
#define VK_IMAGE_VIEW_TYPE_CUBE 0
#define VK_BORDER_COLOR_INT_OPAQUE_BLACK 0
#define VK_SAMPLER_MIPMAP_MODE_LINEAR 0
#define VK_BUFFER_USAGE_TRANSFER_DST_BIT 0
#define VK_BUFFER_USAGE_VERTEX_BUFFER_BIT 0
#define VK_BUFFER_USAGE_INDEX_BUFFER_BIT 0
#define VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT 0
#define VK_BUFFER_USAGE_TRANSFER_SRC_BIT 0
#define VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT 0
#define VK_FENCE_CREATE_SIGNALED_BIT 0
#define VK_KHR_SURFACE_EXTENSION_NAME "VK_KHR_surface"
#define VK_KHR_ANDROID_SURFACE_EXTENSION_NAME "VK_KHR_android_surface"
#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
#define VK_KHR_XLIB_SURFACE_EXTENSION_NAME "VK_KHR_xlib_surface"
#define VK_KHR_SWAPCHAIN_EXTENSION_NAME "VK_KHR_swapchain"
#define VK_API_VERSION_1_1 0
#define VK_MAKE_VERSION(a,b,c) 0
#define VK_SUCCESS 0
#define UINT64_MAX 0xFFFFFFFFFFFFFFFFULL
inline VkResult vkCreateInstance(const void*, const void*, VkInstance*) { return VK_SUCCESS; }
inline void vkDestroyInstance(VkInstance, const void*) {}
inline VkResult vkEnumeratePhysicalDevices(VkInstance, uint32_t*, VkPhysicalDevice*) { return VK_SUCCESS; }
inline void vkGetPhysicalDeviceProperties(VkPhysicalDevice, VkPhysicalDeviceProperties*) {}
inline void vkGetPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice, uint32_t*, VkQueueFamilyProperties*) {}
inline VkResult vkCreateDevice(VkPhysicalDevice, const void*, const void*, VkDevice*) { return VK_SUCCESS; }
inline void vkDestroyDevice(VkDevice, const void*) {}
inline void vkGetDeviceQueue(VkDevice, uint32_t, uint32_t, VkQueue*) {}
inline VkResult vkCreateCommandPool(VkDevice, const void*, const void*, VkCommandPool*) { return VK_SUCCESS; }
inline void vkDestroyCommandPool(VkDevice, VkCommandPool, const void*) {}
inline VkResult vkAllocateCommandBuffers(VkDevice, const void*, VkCommandBuffer*) { return VK_SUCCESS; }
inline VkResult vkCreateSemaphore(VkDevice, const void*, const void*, VkSemaphore*) { return VK_SUCCESS; }
inline void vkDestroySemaphore(VkDevice, VkSemaphore, const void*) {}
inline VkResult vkCreateFence(VkDevice, const void*, const void*, VkFence*) { return VK_SUCCESS; }
inline void vkDestroyFence(VkDevice, VkFence, const void*) {}
inline VkResult vkCreateBuffer(VkDevice, const void*, const void*, VkBuffer*) { return VK_SUCCESS; }
inline void vkDestroyBuffer(VkDevice, VkBuffer, const void*) {}
inline void vkGetBufferMemoryRequirements(VkDevice, VkBuffer, VkMemoryRequirements*) {}
inline VkResult vkAllocateMemory(VkDevice, const void*, const void*, VkDeviceMemory*) { return VK_SUCCESS; }
inline void vkFreeMemory(VkDevice, VkDeviceMemory, const void*) {}
inline VkResult vkBindBufferMemory(VkDevice, VkBuffer, VkDeviceMemory, VkDeviceSize) { return VK_SUCCESS; }
inline VkResult vkCreateImage(VkDevice, const void*, const void*, VkImage*) { return VK_SUCCESS; }
inline void vkDestroyImage(VkDevice, VkImage, const void*) {}
inline void vkGetImageMemoryRequirements(VkDevice, VkImage, VkMemoryRequirements*) {}
inline VkResult vkBindImageMemory(VkDevice, VkImage, VkDeviceMemory, VkDeviceSize) { return VK_SUCCESS; }
inline VkResult vkCreateImageView(VkDevice, const void*, const void*, VkImageView*) { return VK_SUCCESS; }
inline void vkDestroyImageView(VkDevice, VkImageView, const void*) {}
inline VkResult vkCreateSampler(VkDevice, const void*, const void*, VkSampler*) { return VK_SUCCESS; }
inline void vkDestroySampler(VkDevice, VkSampler, const void*) {}
inline VkResult vkCreateShaderModule(VkDevice, const void*, const void*, VkShaderModule*) { return VK_SUCCESS; }
inline void vkDestroyShaderModule(VkDevice, VkShaderModule, const void*) {}
inline VkResult vkCreatePipelineLayout(VkDevice, const void*, const void*, VkPipelineLayout*) { return VK_SUCCESS; }
inline void vkDestroyPipelineLayout(VkDevice, VkPipelineLayout, const void*) {}
inline void vkDestroyPipeline(VkDevice, VkPipeline, const void*) {}
inline VkResult vkBeginCommandBuffer(VkCommandBuffer, const void*) { return VK_SUCCESS; }
inline VkResult vkEndCommandBuffer(VkCommandBuffer) { return VK_SUCCESS; }
inline VkResult vkResetCommandBuffer(VkCommandBuffer, VkFlags) { return VK_SUCCESS; }
inline void vkCmdSetViewport(VkCommandBuffer, uint32_t, uint32_t, const VkViewport*) {}
inline void vkCmdSetScissor(VkCommandBuffer, uint32_t, uint32_t, const VkRect2D*) {}
inline void vkCmdBindVertexBuffers(VkCommandBuffer, uint32_t, uint32_t, const VkBuffer*, const VkDeviceSize*) {}
inline void vkCmdBindIndexBuffer(VkCommandBuffer, VkBuffer, VkDeviceSize, VkIndexType) {}
inline void vkCmdBindPipeline(VkCommandBuffer, VkPipelineBindPoint, VkPipeline) {}
inline void vkCmdDraw(VkCommandBuffer, uint32_t, uint32_t, uint32_t, uint32_t) {}
inline void vkCmdDrawIndexed(VkCommandBuffer, uint32_t, uint32_t, uint32_t, int32_t, uint32_t) {}
inline void vkCmdClearColorImage(VkCommandBuffer, VkImage, VkImageLayout, const VkClearColorValue*, uint32_t, const VkImageSubresourceRange*) {}
inline void vkCmdClearDepthStencilImage(VkCommandBuffer, VkImage, VkImageLayout, const VkClearDepthStencilValue*, uint32_t, const VkImageSubresourceRange*) {}
inline void vkCmdCopyBufferToImage(VkCommandBuffer, VkBuffer, VkImage, VkImageLayout, uint32_t, const VkBufferImageCopy*) {}
inline void vkCmdPipelineBarrier(VkCommandBuffer, VkPipelineStageFlags, VkPipelineStageFlags, VkFlags, uint32_t, const void*, uint32_t, const void*, uint32_t, const void*) {}
inline VkResult vkQueueSubmit(VkQueue, uint32_t, const void*, VkFence) { return VK_SUCCESS; }
inline VkResult vkWaitForFences(VkDevice, uint32_t, const VkFence*, int, uint64_t) { return VK_SUCCESS; }
inline VkResult vkResetFences(VkDevice, uint32_t, const VkFence*) { return VK_SUCCESS; }
inline VkResult vkDeviceWaitIdle(VkDevice) { return VK_SUCCESS; }
inline VkResult vkCreateSwapchainKHR(VkDevice, const void*, const void*, VkSwapchainKHR*) { return VK_SUCCESS; }
inline void vkDestroySwapchainKHR(VkDevice, VkSwapchainKHR, const void*) {}
inline VkResult vkDestroySurfaceKHR(VkInstance, VkSurfaceKHR, const void*) { return VK_SUCCESS; }
inline void vkGetPhysicalDeviceMemoryProperties(VkPhysicalDevice, VkPhysicalDeviceMemoryProperties*) {}
#endif
#else
#include <vulkan/vulkan.h>
#define SONIC_HAS_VULKAN 1
#endif

#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace sonic_native {

class VulkanRHIBuffer : public RHIBuffer {
public:
    VulkanRHIBuffer(VkDevice device, VkBuffer buffer, VkDeviceMemory memory, uint64_t size, void* mapped)
        : device_(device), buffer_(buffer), memory_(memory), size_(size), mapped_(mapped) {}
    ~VulkanRHIBuffer() override;
    
    uint64_t GetSize() const override { return size_; }
    void* Map() override { return mapped_; }
    void Unmap() override {}
    
    VkBuffer GetVkBuffer() const { return buffer_; }
    
private:
    VkDevice device_;
    VkBuffer buffer_;
    VkDeviceMemory memory_;
    uint64_t size_;
    void* mapped_;
};

class VulkanRHITexture : public RHITexture {
public:
    VulkanRHITexture(VkDevice device, VkImage image, VkDeviceMemory memory, VkImageView view, uint32_t w, uint32_t h, RHIFormat fmt)
        : device_(device), image_(image), memory_(memory), view_(view), width_(w), height_(h), format_(fmt) {}
    ~VulkanRHITexture() override;
    
    uint32_t GetWidth() const override { return width_; }
    uint32_t GetHeight() const override { return height_; }
    RHIFormat GetFormat() const override { return format_; }
    
    VkImage GetVkImage() const { return image_; }
    VkImageView GetVkView() const { return view_; }
    
private:
    VkDevice device_;
    VkImage image_;
    VkDeviceMemory memory_;
    VkImageView view_;
    uint32_t width_, height_;
    RHIFormat format_;
};

class VulkanRHISampler : public RHISampler {
public:
    VulkanRHISampler(VkDevice device, VkSampler sampler) : device_(device), sampler_(sampler) {}
    ~VulkanRHISampler() override;
    
    VkSampler GetVkSampler() const { return sampler_; }
    
private:
    VkDevice device_;
    VkSampler sampler_;
};

class VulkanRHIShader : public RHIShader {
public:
    VulkanRHIShader(VkDevice device, VkShaderModule module) : device_(device), module_(module) {}
    ~VulkanRHIShader() override;
    
    VkShaderModule GetVkModule() const { return module_; }
    
private:
    VkDevice device_;
    VkShaderModule module_;
};

class VulkanRHIPipeline : public RHIPipeline {
public:
    VulkanRHIPipeline(VkDevice device, VkPipeline pipeline, VkPipelineLayout layout)
        : device_(device), pipeline_(pipeline), layout_(layout) {}
    ~VulkanRHIPipeline() override;
    
    VkPipeline GetVkPipeline() const { return pipeline_; }
    VkPipelineLayout GetVkLayout() const { return layout_; }
    
private:
    VkDevice device_;
    VkPipeline pipeline_;
    VkPipelineLayout layout_;
};

class VulkanRHICommandList : public RHICommandList {
public:
    VulkanRHICommandList(VkDevice device, VkCommandBuffer cmdBuffer);
    
    void SetViewport(const RHIViewport& vp) override;
    void SetScissor(const RHIRect& rect) override;
    void SetRenderTargets(RHITexture* color, RHITexture* depth) override;
    void ClearColor(RHITexture* tex, const float color[4]) override;
    void ClearDepthStencil(RHITexture* tex, float depth, uint8_t stencil) override;
    void SetVertexBuffer(uint32_t slot, RHIBuffer* buffer, uint32_t stride, uint64_t offset) override;
    void SetIndexBuffer(RHIBuffer* buffer, RHIFormat format, uint64_t offset) override;
    void SetPipeline(RHIPipeline* pipeline) override;
    void SetConstantBuffer(uint32_t slot, RHIBuffer* buffer, uint64_t offset) override;
    void SetTexture(uint32_t slot, RHITexture* texture) override;
    void SetSampler(uint32_t slot, RHISampler* sampler) override;
    void Draw(const RHIDrawArgs& args) override;
    void DrawIndexed(const RHIIndexedDrawArgs& args) override;
    void CopyBufferToTexture(RHITexture* dst, RHIBuffer* src, uint64_t srcOffset, uint32_t mip, uint32_t slice) override;
    void Barrier(RHITexture* tex, RHIResourceState before, RHIResourceState after) override;
    void FlushBarriers() override;
    
    VkCommandBuffer GetVkCommandBuffer() const { return cmdBuffer_; }
    void Begin();
    void End();
    void Reset();
    
private:
    VkDevice device_;
    VkCommandBuffer cmdBuffer_;
    
    struct BoundState {
        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout layout = VK_NULL_HANDLE;
        RHITexture* renderTarget = nullptr;
        RHITexture* depthStencil = nullptr;
        bool renderPassBegun = false;
    } bound_;
    
    // Barrier batching
    struct BarrierOp {
        RHITexture* texture;
        RHIResourceState before, after;
    };
    std::vector<BarrierOp> pendingBarriers_;
};

class VulkanRHIDevice : public RHIDevice {
public:
    VulkanRHIDevice();
    ~VulkanRHIDevice() override;
    
    bool Initialize(void* windowHandle) override;
    void Shutdown() override;
    
    RHIBuffer* CreateBuffer(const RHIBufferDesc& desc) override;
    RHITexture* CreateTexture(const RHITextureDesc& desc) override;
    RHISampler* CreateSampler(const RHISamplerDesc& desc) override;
    RHIShader* CreateVertexShader(const void* bytecode, size_t size) override;
    RHIShader* CreatePixelShader(const void* bytecode, size_t size) override;
    RHIPipeline* CreatePipeline(const RHIPipelineDesc& desc) override;
    
    void DestroyBuffer(RHIBuffer* buffer) override;
    void DestroyTexture(RHITexture* tex) override;
    void DestroySampler(RHISampler* sampler) override;
    void DestroyShader(RHIShader* shader) override;
    void DestroyPipeline(RHIPipeline* pipeline) override;
    
    RHICommandList* GetCommandList() override;
    void Submit() override;
    void Present() override;
    void WaitIdle() override;
    
    const char* GetBackendName() const override { return "Vulkan"; }
    
    // Vulkan-specific
    VkDevice GetVkDevice() const { return device_; }
    VkPhysicalDevice GetVkPhysicalDevice() const { return physicalDevice_; }
    VkInstance GetVkInstance() const { return instance_; }
    VkQueue GetVkQueue() const { return queue_; }
    uint32_t GetQueueFamily() const { return queueFamily_; }
    
private:
    bool CreateInstance();
    bool CreateDevice();
    bool CreateSwapchain(void* windowHandle);
    bool CreateCommandPool();
    VkFormat TranslateFormat(RHIFormat fmt);
    VkFilter TranslateFilter(RHIFilter filter);
    VkSamplerAddressMode TranslateAddressMode(RHIAddressMode mode);
    
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue queue_ = VK_NULL_HANDLE;
    uint32_t queueFamily_ = 0;
    
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain_ = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages_;
    std::vector<VkImageView> swapchainViews_;
    uint32_t swapchainImageIndex_ = 0;
    
    VkCommandPool commandPool_ = VK_NULL_HANDLE;
    VkCommandBuffer commandBuffer_ = VK_NULL_HANDLE;
    std::unique_ptr<VulkanRHICommandList> commandList_;
    
    VkSemaphore imageAvailableSemaphore_ = VK_NULL_HANDLE;
    VkSemaphore renderFinishedSemaphore_ = VK_NULL_HANDLE;
    VkFence inFlightFence_ = VK_NULL_HANDLE;
    
    // For Android: use ANativeWindow
    void* windowHandle_ = nullptr;
    
    bool initialized_ = false;
    
    // Memory allocation helper
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags props);
};

} // namespace sonic_native
