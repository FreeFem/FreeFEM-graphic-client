#ifndef INSTANCE_H
#define INSTANCE_H

#include <vector>
#include <vulkan/vulkan.h>
#include "utils.h"

std::vector<const char *> GetRequiredExtensions();
bool createInstance(VkInstance& outInstance, std::vector<const char *> enabledExtensions, std::vector<const char *> enabledLayers);

bool chooseVkPhysicalDevice(const VkInstance instance, VkPhysicalDevice &outPhysicalDevice, const VkSurfaceKHR surface);
bool isDeviceSuitable(VkPhysicalDevice phyDev, const VkSurfaceKHR surface);
familyIndex findQueueFamilies(VkPhysicalDevice phyDev, const VkSurfaceKHR surface);

bool createSurfaceKHR(GLFWwindow *window, const VkInstance instance, VkSurfaceKHR &outSurfaceKHR);

bool createDeviceAndQueue(const VkPhysicalDevice phyDev,
                          const VkSurfaceKHR surface,
                          const std::vector<const char *>& enabledLayers,
                          VkDevice& outDevice,
                          VkQueue& outQueue,
                          uint32_t& outQueueFamilyIndex);

bool createSwapchain(const VkPhysicalDevice phyDev,
                     const VkDevice device,
                     const VkSurfaceKHR surface,
                     GLFWwindow *window,
                     VkSwapchainKHR oldSwapchain,
                     VkSwapchainKHR& outSwapchain,
                     const int ownedSwapchainImages,
                     VkFormat& outSurfaceFormat,
                     const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);

bool getSwapchainImagesAndViews(const VkDevice device,
                                const VkSwapchainKHR swapchain,
                                const VkFormat *surfaceFormat,
                                std::vector<VkImage>& outSwapchainImages,
                                std::vector<VkImageView>& outSwapchainImageViews);

bool createCommandPool(const VkDevice device,
                       const uint32_t queueFamilyIndex,
                       const VkCommandPoolCreateFlagBits createFlagBits,
                       VkCommandPool& outCommandPool);

bool allocateCommandBuffer(const VkDevice device,
                           const VkCommandPool commandPool,
                           const VkCommandBufferLevel commandBufferLevel,
                           VkCommandBuffer& outCommandBuffer);

bool createAndAllocateImage(const VkDevice device,
                            const VkPhysicalDeviceMemoryProperties memProps,
                            const VkBufferUsageFlags imageUsage,
                            const VkMemoryPropertyFlags requiredMemoryProps,
                            const VkFormat imageFormat,
                            const int width,
                            const int height,
                            VkImage& outImage,
                            VkDeviceMemory& outImageMem,
                            VkImageView *outImageViewPtr = 0,
                            VkImageAspectFlags viewSubresoucesAspectMask = 0);

bool createAndAllocateBuffer(const VkDevice device,
                             const VkPhysicalDeviceMemoryProperties memProps,
                             const VkBufferUsageFlags bufferUsage,
                             const VkMemoryPropertyFlags requiredMemProps,
                             const VkDeviceSize bufferSize,
                             VkBuffer& outBuffer,
                             VkDeviceMemory& outBufferMem);

bool createRenderPass(const VkDevice device,
                      const VkFormat swapchainImageFormat,
                      const VkFormat depthBufferFormat,
                      VkRenderPass& outRenderPass);

bool createPipeline(const VkDevice device,
                    const VkRenderPass renderPass,
                    const VkPipelineLayout pipelineLayout,
                    const char *vertexShaderFileName,
                    const char *fragmentShaderFileName,
                    const uint32_t vertexInputBinding,
                    VkPipeline &outPipeline);

#endif // INSTANCE_H