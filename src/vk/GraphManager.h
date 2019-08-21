#ifndef GRAPH_MANAGER_H
#define GRAPH_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../util/utils.h"
#include "CommandBuffer.h"
#include "../core/NativeWindow.h"
#include "Image.h"


namespace gr
{
    struct ManagerInitInfo {
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

/* WIP : Allow to store the enabled extensions on a single uint32_t

    struct VulkanExtensionsBits {
        uint32_t bits;
        const char name[32];
    };
    static const VulkanExtensionsBits supportedExtensions[] =
    {
        {1 << 0, VK_KHR_SURFACE_EXTENSION_NAME},
        {1 << 1, }
    }

    struct VulkanExtensions {
        uint32_t value;

        uint32_t& operator+=(const char *extensionName);
    }

*/
    /**
     * @brief Contain vulkan which don't need to be recreate.
     */
    class Manager {
        public:

            Manager() {}

            ~Manager();

            FORCE_USE_RESULT Error init(const ManagerInitInfo& initInfo);

            inline VkInstance getInstance() const { return m_instance; }
            inline VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
            inline VkDevice getDevice() const { return m_device; }
            inline uint32_t getGraphicsQueueFamily() const {return m_queueIdx; }
            inline VkSurfaceKHR getSurface() const { return m_surface; }
            inline NativeWindow getNativeWindow() const { return *m_window; }
            inline VkPhysicalDeviceMemoryProperties getPhysicalDeviceMemProps() const { return m_memoryProperties; }

            void beginDebugMaker(VkCommandBuffer cmdBuffer, const char *name) const;
            void endDebugMaker(VkCommandBuffer cmdBuffer) const;

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
            VkPhysicalDeviceMemoryProperties m_memoryProperties = {};

            PFN_vkDebugMarkerSetObjectNameEXT m_pfnDebugMarkerSetObjectNameEXT = nullptr;
            PFN_vkCmdDebugMarkerBeginEXT m_pfnCmdDebugMarkerBeginEXT = nullptr;
            PFN_vkCmdDebugMarkerEndEXT m_pfnCmdDebugMarkerEndEXT = nullptr;
            PFN_vkGetShaderInfoAMD m_pfnGetShaderInfoAMD = nullptr;
            PFN_vkCreateDebugUtilsMessengerEXT m_pfnCreateDebugUtilsMessengerEXT = nullptr;

            NativeWindow *m_window = NULL;

            VkSurfaceKHR m_surface;
            VkFormat m_surfaceFormat;

            FORCE_USE_RESULT Error initInstance();
            FORCE_USE_RESULT Error initSurface();
            FORCE_USE_RESULT Error initDevice();
    };
} // namespace gr

#endif // GRAPH_MANAGER_H