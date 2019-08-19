#ifndef VKCONTEXT_H
#define VKCONTEXT_H

#include <vector>
#include <map>
#include <cstdint>
#include <GLFW/glfw3.h>
#include "common.h"

/**
 * @brief Contain all data regarding Vulkan element creation
 */
class vkContext {

    public:
        typedef std::vector<const char *> VulkanExtensions;

        #define NUMBER_OF_FRAME ((uint8_t)(2))

        vkContext() { }

        ~vkContext();

        VkInstance m_instance = VK_NULL_HANDLE;
        VkDebugReportCallbackEXT m_debugCallback = VK_NULL_HANDLE;
        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VulkanExtensions m_extensions = {};
        VkDevice m_device = VK_NULL_HANDLE;
        uint32_t m_queueIdx = UINT32_MAX;
        VkQueue m_queue = VK_NULL_HANDLE;
        VkCommandPool m_cmdPool = VK_NULL_HANDLE;
        VkCommandBuffer m_cmdBuffer = VK_NULL_HANDLE;

        VkPhysicalDeviceProperties m_devProps = {};
        VkPhysicalDeviceFeatures m_devFeatures = {};
        VkPhysicalDeviceDescriptorIndexingFeaturesEXT m_descriptorIndexingFeatures = {};

        PFN_vkDebugMarkerSetObjectNameEXT m_pfnDebugMarkerSetObjectNameEXT = nullptr;
        PFN_vkCmdDebugMarkerBeginEXT m_pfnCmdDebugMarkerBeginEXT = nullptr;
        PFN_vkCmdDebugMarkerEndEXT m_pfnCmdDebugMarkerEndEXT = nullptr;
        PFN_vkGetShaderInfoAMD m_pfnGetShaderInfoAMD = nullptr;

        struct PerFrame {
            VkFence m_presentFence;
            VkSemaphore m_acquireSemaphore;
            VkSemaphore m_renderSemaphore;
            VkCommandBuffer m_presentCmdBuffer;
            bool fenceInitialized;
        };

        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        GLFWwindow *window = NULL;

        PerFrame m_perFrame[NUMBER_OF_FRAME];

        VkPhysicalDeviceMemoryProperties m_memoryProperties;

        USE_RESULT ReturnValue init();
    private:

        USE_RESULT ReturnValue initInternal();
        USE_RESULT ReturnValue initInstance();
        USE_RESULT ReturnValue initSurface();
        USE_RESULT ReturnValue initDevice();

}; // vkContext

#endif // VKCONTEXT_H