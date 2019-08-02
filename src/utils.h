#ifndef UTILS_H
#define UTILS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>

struct familyIndex {
    bool has_graphicFamily;
    uint32_t graphicFamily;
    bool has_presentFamily;
    uint32_t presentFamily;
};

const char *VkResultToStr(const VkResult result);

bool glfwGetWindow(const char *title, int width, int height, GLFWwindow **outWindow);

int findMemoryTypeWithProperties(const VkPhysicalDeviceMemoryProperties memoryProperties,
                                 const uint32_t memoryTypeBits,
                                 const VkMemoryPropertyFlags requiredMemoryProperties);

VkResult createFence(const VkDevice device, VkFence *outFence);

VkResult createSemaphore(const VkDevice device, VkSemaphore *outSemaphore);

bool createFramebuffer(const VkDevice device, const VkRenderPass renderPass, const std::vector<VkImageView>& viewAttachementVector,
                        const int width, const int height, VkFramebuffer *outFramebuffer);

bool loadAndCreateShaderModule(const VkDevice device, const char *filename, VkShaderModule *outShaderModule);

#endif // UTILS_H