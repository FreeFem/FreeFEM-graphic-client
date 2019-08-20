#ifndef GRAPH_MANAGER_H
#define GRAPH_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../util/utils.h"
#include "CommandBuffer.h"
#include "Swapchain.h"
#include "../core/NativeWindow.h"

struct GraphManagerInitInfo {
    NativeWindow& window;
};

struct GPUDeviceCapabilities {
    size_t m_uniformBufferBindOffsetAlignment = UINT32_MAX;
    size_t m_uniformBufferMaxRange = 0;
    size_t m_storageBufferBindOffsetAlignment = UINT32_MAX;
    size_t m_storageBufferMaxRange = 0;
    size_t m_textureBufferBindOffsetAlignment = UINT32_MAX;
    size_t m_textureBufferMaxRange = 0;
    uint32_t m_pushContantsSize = 128;

    uint32_t m_gpuVendor = 0;
    uint8_t m_minorApiVersion = 0;
    uint8_t m_majorApiVersion = 0;
};

class GraphManager {
    public:

        GraphManager() {}

        ~GraphManager();

        FORCE_USE_RESULT Error init(const GraphManagerInitInfo& initInfo);

    private:

        VkInstance m_instance = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT m_debugCallback = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

        VkPhysicalDevice m_physicalDevice;
        std::vector<const char *> m_extensions = {}; // To-Do : store them is a "prettier" way (Bit shift)

        VkDevice m_device = VK_NULL_HANDLE;
        uint32_t m_queueIdx;
        VkQueue m_queue = VK_NULL_HANDLE;

        GPUDeviceCapabilities m_capabilities;
        VkPhysicalDeviceProperties m_devProps = {};
        VkPhysicalDeviceFeatures m_devFeatures = {};
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT m_descriptorIndexingFeatures = {};

        PFN_vkDebugMarkerSetObjectNameEXT m_pfnDebugMarkerSetObjectNameEXT = nullptr;
        PFN_vkCmdDebugMarkerBeginEXT m_pfnCmdDebugMarkerBeginEXT = nullptr;
        PFN_vkCmdDebugMarkerEndEXT m_pfnCmdDebugMarkerEndEXT = nullptr;
        PFN_vkGetShaderInfoAMD m_pfnGetShaderInfoAMD = nullptr;
        PFN_vkCreateDebugUtilsMessengerEXT m_pfnCreateDebugUtilsMessengerEXT = nullptr;

        struct PerFrame {
            VkFence presentFence;
            VkCommandBuffer presentCmdBuffer;
            VkSemaphore imageAcquiredSemaphore;
            VkSemaphore renderingCompletedSemaphore;
            bool fenceInitialized;
        };

        NativeWindow *m_window = NULL;

        VkSurfaceKHR m_surface;
        Swapchain m_swapchain;
        VkFormat m_surfaceFormat;
        uint8_t m_acquiredImageIdx = UINT8_MAX;

        PerFrame m_frames[2];

        VkPhysicalDeviceMemoryProperties m_memoryProperties;

        CommandBufferCreator m_commandBufferCreator;

        SwapchainCreator m_SwapchainCreator;

        FORCE_USE_RESULT Error initInstance();
        FORCE_USE_RESULT Error initSurface();
        FORCE_USE_RESULT Error initDevice();
};

#endif // GRAPH_MANAGER_H