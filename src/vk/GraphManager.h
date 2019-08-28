#ifndef GRAPH_MANAGER_H
#define GRAPH_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../util/utils.h"
#include "../core/NativeWindow.h"
#include "Image.h"
#include "vk_mem_alloc.h"

namespace gr
{
    /**
     * @brief Contains all data used to create a Manager.
     */
    struct ManagerInitInfo {
        NativeWindow& window;
    };

    /**
     * @brief Contains the GPU's capabilities.
     */
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
     * @brief Vulkan object manager.
     */
    class Manager {
        public:

            Manager() {}

            ~Manager();

            /**
             * @brief Initialize a manager.
             *
             * @param const ManagerInitInfo& initInfo[in] - Data used to create the manager.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error init(const ManagerInitInfo& initInfo);

            // @brief Vulkan instance handle getter.
            inline VkInstance getInstance() const { return m_instance; }
            // @brief Vulkan physical device handle getter.
            inline VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
            // @brief Vulakn allocator getter.
            inline VmaAllocator getAllocator() const { return m_allocator; }
            // @brief Vulkan device handle getter.
            inline VkDevice getDevice() const { return m_device; }
            // @brief Vulkan device's queue handle getter.
            inline VkQueue getQueue() const { return m_queue; }
            // @brief Vulkan graphics queue family getter.
            inline uint32_t getGraphicsQueueFamily() const {return m_queueIdx; }
            // @brief Vulkan surface handle getter.
            inline VkSurfaceKHR getSurface() const { return m_surface; }
            // @brief NativeWindow getter.
            inline NativeWindow getNativeWindow() const { return *m_window; }
            // @brief Vulkan physical device memory properties getter.
            inline VkPhysicalDeviceMemoryProperties getPhysicalDeviceMemProps() const { return m_memoryProperties; }

            /**
             * @brief Debugger for commandbuffer.
             */
            void beginDebugMaker(VkCommandBuffer cmdBuffer, const char *name) const;

            /**
             * @brief Debugger for commandbuffer.
             */
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

            VmaAllocator m_allocator;

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

            /**
             * @brief Initialize manager's instance.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initInstance();

            /**
             * @brief Initialize manager's surface.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initSurface();

            /**
             * @brief Initialize manager's device.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initDevice();

            /**
             * @brief Initialize vulkan memory allocator.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initAllocator();
    };
} // namespace gr

#endif // GRAPH_MANAGER_H