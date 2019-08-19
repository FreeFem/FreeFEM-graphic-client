#include <cstring>
#include "vkContext.hpp"
#include "layers.h"

vkContext::~vkContext()
{
    if (m_queue) {
        vkQueueWaitIdle(m_queue);
        m_queue = VK_NULL_HANDLE;
    }
}

ReturnError vkContext::init(const vkContextInitInfo& init)
{
    ReturnError result = initInternal(init);

    if (result != ReturnError::NONE) {
        dprintf(2, "Failed to initialized Vulkan.\n");
        return result;
    }
    return result;
}

ReturnError vkContext::initInternal(const vkContextInitInfo& init)
{
    if (glfwInit() != GLFW_TRUE)
        return ReturnError::FUNCTION_FAILED;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(init.width, init.height, init.title, 0, 0);
    CHECK_MEMORY_ALLOC(window);

    CHECK(initInstance());
    CHECK(initSurface());
    CHECK(initDevice());

	// Set m_r8g8b8ImagesSupported
	{
		VkImageFormatProperties props = {};
		VkResult res = vkGetPhysicalDeviceImageFormatProperties(m_physicalDevice,
			VK_FORMAT_R8G8B8_UNORM,
			VK_IMAGE_TYPE_2D,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			0,
			&props);

		if(res == VK_ERROR_FORMAT_NOT_SUPPORTED)
		{
			dprintf(2, "R8G8B8 Images are not supported. Will workaround this");
			m_r8g8b8ImagesSupported = false;
		}
		else
		{
            CHECK_VK_FNC(res);
			dprintf(2, "R8G8B8 Images are supported");
			m_r8g8b8ImagesSupported = true;
		}
	}

    return ReturnError::NONE;
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

ReturnError vkContext::initInstance()
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

    // Validation Layers
    if (LAYERS_ENABLED) {
        checkValidationLayerSupport(debugLayers);

        createInfo.enabledLayerCount = (uint32_t)debugLayers.size();
        createInfo.ppEnabledLayerNames = debugLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = 0;
    }

    // Extensions
    if (window) {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        for (uint_fast32_t i = 0; i < glfwExtensionCount; i += 1)
            m_extensions.push_back(glfwExtensions[i]);

        if (LAYERS_ENABLED) {
            m_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            m_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        createInfo.enabledExtensionCount = (uint32_t)m_extensions.size();
        createInfo.ppEnabledExtensionNames = m_extensions.data();
    } else {
        return VK_INCOMPLETE;
    }

    CHECK_VK_FNC(vkCreateInstance(&createInfo, 0, &m_instance));

    // Setup debug callback
    if (LAYERS_ENABLED) {
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.pfnCallback = debugReportCallbackEXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT
                        | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        createInfo.pUserData = this;

        PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
            reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
                vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT"));
        if (!vkCreateDebugReportCallbackEXT)
            return VK_INCOMPLETE;
        vkCreateDebugReportCallbackEXT(m_instance, &createInfo, 0, &m_debugCallback);
    }

    // Create physical device
    uint32_t count = 0;
    CHECK_VK_FNC(vkEnumeratePhysicalDevices(m_instance, &count, 0));
    if (count < 1)
        return ReturnError::FUNCTION_FAILED;
    count = 1;
    CHECK_VK_FNC(vkEnumeratePhysicalDevices(m_instance, &count, &m_physicalDevice));

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

    return ReturnError::NONE;
}

ReturnError vkContext::initSurface()
{
    CHECK_VK_FNC(glfwCreateWindowSurface(m_instance, window, 0, &m_surface));
    return ReturnError::NONE;
}

ReturnError vkContext::initDevice()
{
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, 0);

    std::vector<VkQueueFamilyProperties> queueInfos(count);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &count, queueInfos.data());

    uint32_t desiredFamilyIdx = UINT32_MAX;
    const VkQueueFlags DESIRED_QUEUE_FLAGS = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT;

    // Pick the queue family
    for (uint_fast32_t i = 0; i < count; i += 1) {
        if ((queueInfos[i].queueFlags & DESIRED_QUEUE_FLAGS) == DESIRED_QUEUE_FLAGS) {
            VkBool32 supportsPresent = false;
            CHECK_VK_FNC(vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, i, m_surface, &supportsPresent));
            if (supportsPresent) {
                desiredFamilyIdx = i;
                break;
            }
        }
    }
    if (desiredFamilyIdx == UINT32_MAX) {
        return ReturnError::FUNCTION_FAILED;
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
    std::vector<const char *> extensions = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME}};

    if (LAYERS_ENABLED)
        extensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);

    if (m_capabilities.m_gpuVendor == 0x1002 || m_capabilities.m_gpuVendor == 0x1022)
        extensions.push_back(VK_AMD_SHADER_INFO_EXTENSION_NAME);

    deviceCreateInfo.enabledLayerCount = (uint32_t)extensions.size();
    deviceCreateInfo.ppEnabledLayerNames = extensions.data();

    CHECK_VK_FNC(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, 0, &m_device));

    if (LAYERS_ENABLED) {
        m_pfnDebugMarkerSetObjectNameEXT = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>
                    (vkGetDeviceProcAddr(m_device, "vkDebugMarkerSetObjectNameEXT"));
        if (m_pfnDebugMarkerSetObjectNameEXT == 0)
            dprintf(2, "VK_EXT_debug_marker is present but vkDebugMarkerSetObjectNameEXT is not there\n");
        m_pfnCmdDebugMarkerBeginEXT = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>
                    (vkGetDeviceProcAddr(m_device, "vkCmdDebugMarkerBeginEXT"));
        if (m_pfnCmdDebugMarkerBeginEXT)
            dprintf(2, "VK_EXT_debug_marker is present but vkCmdDebugMarkerBeginEXT is not there\n");
        m_pfnCmdDebugMarkerEndEXT = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>
                    (vkGetDeviceProcAddr(m_device, "vkCmdDebugMarkerEndEXT"));
        if (m_pfnCmdDebugMarkerEndEXT)
            dprintf(2, "VK_EXT_debug_marker is present but vkCmdDebugMarkerEndEXT is not there\n");
    }

    if (m_capabilities.m_gpuVendor == 0x1002 || m_capabilities.m_gpuVendor == 0x1022) {
        m_pfnGetShaderInfoAMD = reinterpret_cast<PFN_vkGetShaderInfoAMD>
                    (vkGetDeviceProcAddr(m_device, "vkGetShaderInfoAMD"));
        if (m_pfnGetShaderInfoAMD == 0)
            dprintf(2, "VK_AMD_shader_info is present but vkGetShaderInfoAMD is not there\n");
    }

    return ReturnError::NONE;
}