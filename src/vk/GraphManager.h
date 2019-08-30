#ifndef GRAPH_MANAGER_H
#define GRAPH_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../core/NativeWindow.h"
#include "../util/utils.h"
#include "vk_mem_alloc.h"

namespace FEM {

namespace gr {
/**
 * @brief Contains all data used to create a Manager.
 */
struct ManagerInitInfo {
    NativeWindow &window;
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
class Manager : public NonCopyable {
   public:
    /**
     * @brief Initialize a manager.
     *
     * @param const ManagerInitInfo& initInfo[in] - Data used to create the
     * manager.
     *
     * @return FORCE_USE_RESULT ErrorValues - Returns ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues init(const ManagerInitInfo &initInfo);

    /**
     * @brief Debugger for commandbuffer.
     */
    void beginDebugMaker(VkCommandBuffer cmdBuffer, const char *name) const;

    /**
     * @brief Debugger for commandbuffer.
     */
    void endDebugMaker(VkCommandBuffer cmdBuffer) const;

    VkInstance Instance = VK_NULL_HANDLE;
    std::vector<const char *> Extensions = {};    // To-Do : store them is a "prettier" way (Bit shift)

    VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
    GPUDeviceCapabilities GPUCapabilities;
    VkPhysicalDeviceProperties PhysicalDeviceProperties = {};
    VkPhysicalDeviceFeatures PhysicalDeviceFeatures = {};
    VkPhysicalDeviceMemoryProperties PhysicalDeviceMemoryProperties = {};

    VkDevice Device = VK_NULL_HANDLE;
    VkQueue Queue = VK_NULL_HANDLE;
    uint32_t QueueIdx = UINT32_MAX;

    VmaAllocator Allocator = VK_NULL_HANDLE;

    NativeWindow *Window = NULL;
    VkSurfaceKHR Surface = VK_NULL_HANDLE;
    VkFormat SurfaceFormat;

    VkCommandPool CommandPool = VK_NULL_HANDLE;

   private:
    VkDebugReportCallbackEXT DebugCallback = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT DebugMessenger = VK_NULL_HANDLE;

    PFN_vkDebugMarkerSetObjectNameEXT PfnDebugMarkerSetObjectNameEXT = nullptr;
    PFN_vkCmdDebugMarkerBeginEXT PfnCmdDebugMarkerBeginEXT = nullptr;
    PFN_vkCmdDebugMarkerEndEXT PfnCmdDebugMarkerEndEXT = nullptr;
    PFN_vkGetShaderInfoAMD PfnGetShaderInfoAMD = nullptr;
    PFN_vkCreateDebugUtilsMessengerEXT PfnCreateDebugUtilsMessengerEXT = nullptr;

    /**
     * @brief Initialize manager's instance.
     *
     * @return FORCE_USE_RESULT ErrorValues - Returns ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues initInstance( );

    /**
     * @brief Initialize manager's surface.
     *
     * @return FORCE_USE_RESULT ErrorValues - Returns ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues initSurface( );

    /**
     * @brief Initialize manager's device.
     *
     * @return FORCE_USE_RESULT ErrorValues - Returns ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues initDevice( );

    /**
     * @brief Initialize vulkan memory allocator.
     *
     * @return FORCE_USE_RESULT ErrorValues - Returns ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues initAllocator( );

    /**
     * @brief Initialize vulkan command pool
     *
     * @return FORCE_USE_RESULT ErrorValues - Returns ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues initCommandPool( );
};
}    // namespace gr
}    // namespace FEM

#endif    // GRAPH_MANAGER_H