#include <cstring>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "GraphManager.h"
#include "layers.h"

gr::Manager::~Manager()
{
}

Error gr::Manager::init(const ManagerInitInfo& initInfo)
{
    m_window = &initInfo.window;

    if (initInstance() != Error::NONE)
        return Error::FUNCTION_FAILED;
    if (initSurface() != Error::NONE)
        return Error::FUNCTION_FAILED;
    if (initDevice() != Error::NONE)
        return Error::FUNCTION_FAILED;
    vkGetDeviceQueue(m_device, m_queueIdx, 0, &m_queue);

    return Error::NONE;
}

static void checkValidationLayerSupport(std::vector<const char *> enabledLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, 0);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    printf("Activated Layers :\n");
    auto ite = enabledLayers.begin();
    for (const char *layerName : enabledLayers) {
        bool layerFound = false;
        for (const auto& layerProps : availableLayers) {
            if (strcmp(layerName, layerProps.layerName) == 0) {
                printf("* %s\n", layerProps.layerName);
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            enabledLayers.erase(ite);
        ++ite;
    }
}

Error gr::Manager::initInstance()
{
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

    createInfo.enabledLayerCount = (uint32_t)debugLayers.size();
    createInfo.ppEnabledLayerNames = debugLayers.data();
    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
#else
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = 0;
#endif

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    for (uint_fast32_t i = 0; i < glfwExtensionCount; i += 1)
        m_extensions.push_back(glfwExtensions[i]);
#ifdef _DEBUG
    m_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    createInfo.enabledExtensionCount = (uint32_t)m_extensions.size();
    createInfo.ppEnabledExtensionNames = m_extensions.data();

    if (vkCreateInstance(&createInfo, 0, &m_instance) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
#ifdef _DEBUG
    VkDebugReportCallbackCreateInfoEXT createInfoDebug = {};
    createInfoDebug.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfoDebug.pfnCallback = debugReportCallbackEXT;
    createInfoDebug.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT
                    | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    createInfoDebug.pUserData = this;

    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT"));
    if (!vkCreateDebugReportCallbackEXT)
        return VK_INCOMPLETE;
    vkCreateDebugReportCallbackEXT(m_instance, &createInfoDebug, 0, &m_debugCallback);
#endif
    VkResult result;

    // Create physical device (Dump way of picking the device in the list, will rework that piece of code later)
    uint32_t count = 0;
    result = vkEnumeratePhysicalDevices(m_instance, &count, 0);
    if (result != VK_SUCCESS || count < 1)
        return Error::FUNCTION_FAILED;
    count = 1;
    result = vkEnumeratePhysicalDevices(m_instance, &count, &m_physicalDevice);
    if (result != VK_SUCCESS)
        return Error::FUNCTION_FAILED;

    vkGetPhysicalDeviceProperties(m_physicalDevice, &m_devProps);

    m_capabilities.m_gpuVendor = m_devProps.vendorID;

    vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_devFeatures);
    m_capabilities.m_uniformBufferBindOffsetAlignment = std::max<uint32_t>(16, m_devProps.limits.minUniformBufferOffsetAlignment);
    m_capabilities.m_storageBufferMaxRange = m_devProps.limits.maxUniformBufferRange;
    m_capabilities.m_storageBufferBindOffsetAlignment = std::max<uint32_t>(16, m_devProps.limits.minStorageBufferOffsetAlignment);
	m_capabilities.m_storageBufferMaxRange = m_devProps.limits.maxStorageBufferRange;
	m_capabilities.m_textureBufferBindOffsetAlignment = std::max<uint32_t>(16, m_devProps.limits.minTexelBufferOffsetAlignment);
	m_capabilities.m_textureBufferMaxRange = UINT32_MAX;

	m_capabilities.m_majorApiVersion = vulkanMajor;
	m_capabilities.m_minorApiVersion = vulkanMinor;

    return Error::NONE;
}

Error gr::Manager::initSurface()
{
    if (glfwCreateWindowSurface(m_instance, m_window->getNativeWindow(), 0, &m_surface) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

Error gr::Manager::initDevice()
{
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, 0);

    std::vector<VkQueueFamilyProperties> queueInfos(count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, queueInfos.data());

    uint32_t desiredFamilyIdx = UINT32_MAX;
    const VkQueueFlags DESIRED_QUEUE_FLAGS = VK_QUEUE_GRAPHICS_BIT;

    vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_devFeatures);
    vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &m_memoryProperties);

    // Pick the queue family
    for (uint_fast32_t i = 0; i < count; i += 1) {
        if ((queueInfos[i].queueFlags & DESIRED_QUEUE_FLAGS) == DESIRED_QUEUE_FLAGS) {
            VkBool32 supportsPresent = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, i, m_surface, &supportsPresent);
            if (supportsPresent) {
                desiredFamilyIdx = i;
                break;
            }
        }
    }
    if (desiredFamilyIdx == UINT32_MAX) {
        return Error::FUNCTION_FAILED;
    }

    m_queueIdx = desiredFamilyIdx;

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
    deviceCreateInfo.pEnabledFeatures = &m_devFeatures;

    // Extensions
    std::vector<const char *> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef _DEBUG
    extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);

    if (m_capabilities.m_gpuVendor == 0x1002 || m_capabilities.m_gpuVendor == 0x1022)
        extensions.push_back(VK_AMD_SHADER_INFO_EXTENSION_NAME);

    deviceCreateInfo.enabledLayerCount = (uint32_t)debugLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = debugLayers.data();
#endif

    deviceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = extensions.data();

    vkCreateDevice(m_physicalDevice, &deviceCreateInfo, 0, &m_device);

#ifdef _DEBUG
    m_pfnDebugMarkerSetObjectNameEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>
                (vkGetDeviceProcAddr(m_device, "vkDebugMarkerSetObjectNameEXT"));
    if (m_pfnDebugMarkerSetObjectNameEXT == 0)
        dprintf(2, "VK_EXT_debug_marker is present but vkDebugMarkerSetObjectNameEXT is not there\n");
    m_pfnCmdDebugMarkerBeginEXT = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>
                (vkGetDeviceProcAddr(m_device, "vkCmdDebugMarkerBeginEXT"));
    if (m_pfnCmdDebugMarkerBeginEXT == 0)
        dprintf(2, "VK_EXT_debug_marker is present but vkCmdDebugMarkerBeginEXT is not there\n");
    m_pfnCmdDebugMarkerEndEXT = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>
                (vkGetDeviceProcAddr(m_device, "vkCmdDebugMarkerEndEXT"));
    if (m_pfnCmdDebugMarkerEndEXT == 0)
        dprintf(2, "VK_EXT_debug_marker is present but vkCmdDebugMarkerEndEXT is not there\n");
    m_pfnCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>
                (vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
    if (m_pfnCreateDebugUtilsMessengerEXT) {
        VkDebugUtilsMessengerCreateInfoEXT dcreateInfo = {};
        populateDebugMessengerCreateInfo(dcreateInfo);
        m_pfnCreateDebugUtilsMessengerEXT(m_instance, &dcreateInfo, 0, &m_DebugMessenger);
    }
    if (m_capabilities.m_gpuVendor == 0x1002 || m_capabilities.m_gpuVendor == 0x1022) {
        m_pfnGetShaderInfoAMD = reinterpret_cast<PFN_vkGetShaderInfoAMD>
                    (vkGetDeviceProcAddr(m_device, "vkGetShaderInfoAMD"));
        if (m_pfnGetShaderInfoAMD == 0)
            dprintf(2, "VK_AMD_shader_info is present but vkGetShaderInfoAMD is not there\n");
    }
#endif

    return Error::NONE;
}

void gr::Manager::beginDebugMaker(VkCommandBuffer cmdBuffer, const char *name) const
{
    if (m_pfnCmdDebugMarkerBeginEXT) {
        VkDebugMarkerMarkerInfoEXT markerInfo = {};
        markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
        markerInfo.color[0] = 1.0f;
        markerInfo.pMarkerName = (name == 0 || strlen(name) == 0) ? name : "Unnamed";
        m_pfnCmdDebugMarkerBeginEXT(cmdBuffer, &markerInfo);
    }
}

void gr::Manager::endDebugMaker(VkCommandBuffer cmdBuffer) const
{
    if (m_pfnCmdDebugMarkerEndEXT)
        m_pfnCmdDebugMarkerEndEXT(cmdBuffer);
}