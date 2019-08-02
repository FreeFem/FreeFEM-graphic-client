#include "utils.h"

const char *VkResultToStr(const VkResult result)
{
    #define MAKE_CASE(resultcode) case resultcode : return #resultcode

	switch(result)
	{
		MAKE_CASE(VK_SUCCESS);
		MAKE_CASE(VK_NOT_READY);
		MAKE_CASE(VK_TIMEOUT);
		MAKE_CASE(VK_EVENT_SET);
		MAKE_CASE(VK_EVENT_RESET);
		MAKE_CASE(VK_INCOMPLETE);
		MAKE_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
		MAKE_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		MAKE_CASE(VK_ERROR_INITIALIZATION_FAILED);
		MAKE_CASE(VK_ERROR_DEVICE_LOST);
		MAKE_CASE(VK_ERROR_MEMORY_MAP_FAILED);
		MAKE_CASE(VK_ERROR_LAYER_NOT_PRESENT);
		MAKE_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
		MAKE_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
		MAKE_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
		MAKE_CASE(VK_ERROR_TOO_MANY_OBJECTS);
		MAKE_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
		MAKE_CASE(VK_ERROR_SURFACE_LOST_KHR);
		MAKE_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
		MAKE_CASE(VK_SUBOPTIMAL_KHR);
		MAKE_CASE(VK_ERROR_OUT_OF_DATE_KHR);
		MAKE_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
		MAKE_CASE(VK_ERROR_VALIDATION_FAILED_EXT);

		default:
		    return "INVALID VKRETURN";
	}
	#undef MAKE_CASE
}

bool glfwGetWindow(const char *title, int width, int height, GLFWwindow **outWindow)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    *outWindow = glfwCreateWindow(width, height, title, 0, 0);
    if (outWindow)
        return true;
    return false;
}

int findMemoryTypeWithProperties(const VkPhysicalDeviceMemoryProperties memoryProperties,
                                 const uint32_t memoryTypeBits,
                                 const VkMemoryPropertyFlags requiredMemoryProperties)
{
    uint32_t typeBits = memoryTypeBits;

    uint32_t len = std::min(memoryProperties.memoryTypeCount, 32u);
    for (uint32_t i = 0; i < len; i += 1) {
        if ((typeBits & 1) == 1) {
            if ((memoryProperties.memoryTypes[i].propertyFlags & requiredMemoryProperties) == requiredMemoryProperties)
                return (int)i;
        }
        typeBits >>= 1;
    }
    return -1;
}

VkResult createFence(const VkDevice device, VkFence *outFence)
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = 0;
    fenceCreateInfo.flags = 0;

    return vkCreateFence(device, &fenceCreateInfo, 0, outFence);
}

VkResult createSemaphore(const VkDevice device, VkSemaphore *outSemaphore)
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = 0;
    semaphoreCreateInfo.flags = 0;

    return vkCreateSemaphore(device, &semaphoreCreateInfo, 0, outSemaphore);
}

bool createFramebuffer(const VkDevice device, const VkRenderPass renderPass, const std::vector<VkImageView>& viewAttachementVector,
                        const int width, const int height, VkFramebuffer *outFramebuffer)
{
    VkResult result;

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.flags = 0;
    createInfo.renderPass = renderPass;
    createInfo.attachmentCount = (uint32_t)viewAttachementVector.size();
    createInfo.pAttachments = viewAttachementVector.data();
    createInfo.width = width;
    createInfo.height = height;
    createInfo.layers = 1;

    result = vkCreateFramebuffer(device, &createInfo, 0, outFramebuffer);

    if (result != VK_SUCCESS)
        return false;
    return false;
}

bool loadAndCreateShaderModule(const VkDevice device, const char *filename, VkShaderModule *outShaderModule)
{
    VkResult result;

    std::ifstream inFile;
    inFile.open(filename, std::ios_base::binary | std::ios_base::ate);

    if (!inFile)
        return false;
    size_t fileSize = inFile.tellg();
    std::vector<char> fileContents(fileSize);

    inFile.seekg(0, std::ios::beg);
    bool readStat = bool(inFile.read(fileContents.data(), fileSize));
    inFile.close();

    if (!readStat)
        return false;
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.flags = 0;
    createInfo.codeSize = fileSize;
    createInfo.pCode = reinterpret_cast<uint32_t *>(fileContents.data());

    result = vkCreateShaderModule(device, &createInfo, 0, outShaderModule);
    if (result)
        return true;
    return false;
}