#include <cstring>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../util/Logger.h"
#include "GraphManager.h"
#include "Layers.h"

namespace FEM {
namespace gr {

ErrorValues Manager::init(const ManagerInitInfo &initInfo) {
    Window = &initInfo.window;

    if (initInstance( ) != ErrorValues::NONE) return ErrorValues::FUNCTION_FAILED;
    if (initSurface( ) != ErrorValues::NONE) return ErrorValues::FUNCTION_FAILED;
    if (initDevice( ) != ErrorValues::NONE) return ErrorValues::FUNCTION_FAILED;
    if (initAllocator( )) return ErrorValues::FUNCTION_FAILED;
    vkGetDeviceQueue(Device, QueueIdx, 0, &Queue);
    if (initCommandPool( )) return ErrorValues::FUNCTION_FAILED;
    return ErrorValues::NONE;
}

static void checkValidationLayerSupport(std::vector<const char *> enabledLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, 0);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data( ));

    auto ite = enabledLayers.begin( );
    for (const char *layerName : enabledLayers) {
        bool layerFound = false;
        for (const auto &layerProps : availableLayers) {
            if (strcmp(layerName, layerProps.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) enabledLayers.erase(ite);
        ++ite;
    }
}

ErrorValues Manager::initInstance( ) {
    const uint32_t vulkanMinor = 0;
    const uint32_t vulkanMajor = 1;

    // Create the instance
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "No Name";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "FreeFEM++ Graphic Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

#ifdef _DEBUG
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    checkValidationLayerSupport(debugLayers);

    createInfo.enabledLayerCount = (uint32_t)debugLayers.size( );
    createInfo.ppEnabledLayerNames = debugLayers.data( );
    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = 0;
#endif

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (uint_fast32_t i = 0; i < glfwExtensionCount; i += 1) Extensions.push_back(glfwExtensions[i]);
#ifdef _DEBUG
    Extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    createInfo.enabledExtensionCount = (uint32_t)Extensions.size( );
    createInfo.ppEnabledExtensionNames = Extensions.data( );

    if (vkCreateInstance(&createInfo, 0, &Instance)) {
        LOGE(FILE_LOCATION( ), "Failed to create VkInstance.");
        return ErrorValues::FUNCTION_FAILED;
    }
#ifdef _DEBUG
    VkDebugReportCallbackCreateInfoEXT createInfoDebug = {};
    createInfoDebug.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfoDebug.pfnCallback = debugReportCallbackEXT;
    createInfoDebug.flags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    createInfoDebug.pUserData = this;

    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(Instance, "vkCreateDebugReportCallbackEXT"));
    if (!vkCreateDebugReportCallbackEXT) return VK_INCOMPLETE;
    vkCreateDebugReportCallbackEXT(Instance, &createInfoDebug, 0, &DebugCallback);
#endif
    VkResult result;

    // Create physical device (Dump way of picking the device in the list, will
    // rework that piece of code later)
    uint32_t count = 0;
    result = vkEnumeratePhysicalDevices(Instance, &count, 0);
    if (result != VK_SUCCESS || count < 1) {
        LOGE(FILE_LOCATION( ), "Couldn't enumerate GPUs.");
        return ErrorValues::FUNCTION_FAILED;
    }
    count = 1;
    result = vkEnumeratePhysicalDevices(Instance, &count, &PhysicalDevice);
    if (result != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Couldn't enumerate GPUs.");
        return ErrorValues::FUNCTION_FAILED;
    }
    vkGetPhysicalDeviceProperties(PhysicalDevice, &PhysicalDeviceProperties);

    GPUCapabilities.m_gpuVendor = PhysicalDeviceProperties.vendorID;

    vkGetPhysicalDeviceFeatures(PhysicalDevice, &PhysicalDeviceFeatures);
    GPUCapabilities.m_uniformBufferBindOffsetAlignment =
        std::max<uint32_t>(16, PhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment);
    GPUCapabilities.m_storageBufferMaxRange = PhysicalDeviceProperties.limits.maxUniformBufferRange;
    GPUCapabilities.m_storageBufferBindOffsetAlignment =
        std::max<uint32_t>(16, PhysicalDeviceProperties.limits.minStorageBufferOffsetAlignment);
    GPUCapabilities.m_storageBufferMaxRange = PhysicalDeviceProperties.limits.maxStorageBufferRange;
    GPUCapabilities.m_textureBufferBindOffsetAlignment =
        std::max<uint32_t>(16, PhysicalDeviceProperties.limits.minTexelBufferOffsetAlignment);
    GPUCapabilities.m_textureBufferMaxRange = UINT32_MAX;

    GPUCapabilities.m_majorApiVersion = vulkanMajor;
    GPUCapabilities.m_minorApiVersion = vulkanMinor;

    return ErrorValues::NONE;
}

ErrorValues Manager::initSurface( ) {
    if (glfwCreateWindowSurface(Instance, Window->getNativeWindow( ), 0, &Surface) != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "GLFW failed to create VkSurfaceKHR.");
        return ErrorValues::FUNCTION_FAILED;
    }
    return ErrorValues::NONE;
}

ErrorValues Manager::initDevice( ) {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &count, 0);

    std::vector<VkQueueFamilyProperties> queueInfos(count);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &count, queueInfos.data( ));

    uint32_t desiredFamilyIdx = UINT32_MAX;
    const VkQueueFlags DESIRED_QUEUE_FLAGS = VK_QUEUE_GRAPHICS_BIT;

    vkGetPhysicalDeviceFeatures(PhysicalDevice, &PhysicalDeviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &PhysicalDeviceMemoryProperties);

    // Pick the queue family
    for (uint_fast32_t i = 0; i < count; i += 1) {
        if ((queueInfos[i].queueFlags & DESIRED_QUEUE_FLAGS) == DESIRED_QUEUE_FLAGS) {
            VkBool32 supportsPresent = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &supportsPresent);
            if (supportsPresent) {
                desiredFamilyIdx = i;
                break;
            }
        }
    }
    if (desiredFamilyIdx == UINT32_MAX) {
        return ErrorValues::FUNCTION_FAILED;
    }

    QueueIdx = desiredFamilyIdx;

    float priority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = desiredFamilyIdx;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &priority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &PhysicalDeviceFeatures;

    // Extensions
    std::vector<const char *> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef _DEBUG
    extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);

    if (GPUCapabilities.m_gpuVendor == 0x1002 || GPUCapabilities.m_gpuVendor == 0x1022)
        extensions.push_back(VK_AMD_SHADER_INFO_EXTENSION_NAME);

    deviceCreateInfo.enabledLayerCount = (uint32_t)debugLayers.size( );
    deviceCreateInfo.ppEnabledLayerNames = debugLayers.data( );
#endif

    deviceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size( );
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data( );

    vkCreateDevice(PhysicalDevice, &deviceCreateInfo, 0, &Device);

#ifdef _DEBUG
    PfnDebugMarkerSetObjectNameEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(
        vkGetDeviceProcAddr(Device, "vkDebugMarkerSetObjectNameEXT"));
    if (PfnDebugMarkerSetObjectNameEXT == 0)
        dprintf(2,
                "VK_EXT_debug_marker is present but "
                "vkDebugMarkerSetObjectNameEXT is not there\n");
    PfnCmdDebugMarkerBeginEXT =
        reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(Device, "vkCmdDebugMarkerBeginEXT"));
    if (PfnCmdDebugMarkerBeginEXT == 0)
        dprintf(2,
                "VK_EXT_debug_marker is present but vkCmdDebugMarkerBeginEXT "
                "is not there\n");
    PfnCmdDebugMarkerEndEXT =
        reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(Device, "vkCmdDebugMarkerEndEXT"));
    if (PfnCmdDebugMarkerEndEXT == 0)
        dprintf(2,
                "VK_EXT_debug_marker is present but vkCmdDebugMarkerEndEXT is "
                "not there\n");
    PfnCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT"));
    if (PfnCreateDebugUtilsMessengerEXT) {
        VkDebugUtilsMessengerCreateInfoEXT dcreateInfo = {};
        populateDebugMessengerCreateInfo(dcreateInfo);
        PfnCreateDebugUtilsMessengerEXT(Instance, &dcreateInfo, 0, &DebugMessenger);
    }
    if (GPUCapabilities.m_gpuVendor == 0x1002 || GPUCapabilities.m_gpuVendor == 0x1022) {
        PfnGetShaderInfoAMD =
            reinterpret_cast<PFN_vkGetShaderInfoAMD>(vkGetDeviceProcAddr(Device, "vkGetShaderInfoAMD"));
        if (PfnGetShaderInfoAMD == 0)
            dprintf(2,
                    "VK_AMD_shader_info is present but vkGetShaderInfoAMD is "
                    "not there\n");
    }
#endif

    return ErrorValues::NONE;
}

ErrorValues Manager::initAllocator( ) {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.device = Device;
    allocatorInfo.physicalDevice = PhysicalDevice;

    if (vmaCreateAllocator(&allocatorInfo, &Allocator)) {
        LOGE(FILE_LOCATION( ), "Failed to create wmaAllocator.");
        return ErrorValues::FUNCTION_FAILED;
    }
    return ErrorValues::NONE;
}

ErrorValues Manager::initCommandPool( ) {
    VkCommandPoolCreateInfo createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = QueueIdx;

    if (vkCreateCommandPool(Device, &createInfo, 0, &CommandPool)) {
        LOGE(FILE_LOCATION( ), "Failed to create VkCommandPool.");
        return ErrorValues::FUNCTION_FAILED;
    }
    return ErrorValues::NONE;
}

void Manager::beginDebugMaker(VkCommandBuffer cmdBuffer, const char *name) const {
    if (PfnCmdDebugMarkerBeginEXT) {
        VkDebugMarkerMarkerInfoEXT markerInfo = {};
        markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
        markerInfo.color[0] = 1.0f;
        markerInfo.pMarkerName = (name == 0 || strlen(name) == 0) ? name : "Unnamed";
        PfnCmdDebugMarkerBeginEXT(cmdBuffer, &markerInfo);
    }
}

void Manager::endDebugMaker(VkCommandBuffer cmdBuffer) const {
    if (PfnCmdDebugMarkerEndEXT) PfnCmdDebugMarkerEndEXT(cmdBuffer);
}

void Manager::destroy()
{
    vmaDestroyAllocator(Allocator);
    vkDestroyCommandPool(Device, CommandPool, 0);
    vkDestroyDevice(Device, 0);
#ifdef _DEBUG
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func)
        func(Instance, DebugMessenger, 0);
    auto func1 = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(Instance, "vkDestroyDebugReportCallbackEXT");
    if (func1)
        func1(Instance, DebugCallback, 0);
#endif

    vkDestroySurfaceKHR(Instance, Surface, 0);
    vkDestroyInstance(Instance, 0);
}

}    // namespace gr
}    // namespace FEM