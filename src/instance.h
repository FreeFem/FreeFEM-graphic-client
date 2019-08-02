#ifndef INSTANCE_H
#define INSTANCE_H

#include <vector>
#include <vulkan/vulkan.h>
#include "utils.h"

std::vector<const char *> GetRequiredExtensions();
bool createInstance(VkInstance *outInstance, std::vector<const char *> enabledExtensions, std::vector<const char *> enabledLayers);

bool chooseVkPhysicalDevice(const VkInstance instance, VkPhysicalDevice *outPhysicalDevice, const VkSurfaceKHR surface);
bool isDeviceSuitable(VkPhysicalDevice phyDev, const VkSurfaceKHR surface);
familyIndex findQueueFamilies(VkPhysicalDevice phyDev, const VkSurfaceKHR surface);

bool createSurfaceKHR(GLFWwindow *window, const VkInstance instance, VkSurfaceKHR *outSurfaceKHR);

bool createDeviceAndQueue(const VkPhysicalDevice phyDev, const VkSurfaceKHR surface, const std::vector<const char *>& enabledLayers, VkDevice *outDevice, VkQueue *outQueue, uint32_t *outQueueFamilyIndex);

bool createSwapchain(const VkPhysicalDevice phyDev,
                     const VkDevice device,
                     const VkSurfaceKHR surface,
                     GLFWwindow *window,
                     VkSwapchainKHR oldSwapchain,
                     VkSwapchainKHR *outSwapchain,
                     const int ownedSwapchainImages,
                     VkFormat *outSurfaceFormat,
                     const VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                    );

bool getSwapchainImagesAndViews(const VkDevice device,
                                const VkSwapchainKHR swapchain,
                                const VkFormat *surfaceFormat,
                                std::vector<VkImage>& outSwapchainImages,
                                std::vector<VkImageView>& outSwapchainImageViews
                                );

bool createCommandPool(const VkDevice device,
                       const uint32_t queueFamilyIndex,
                       const VkCommandPoolCreateFlagBits createFlagBits,
                       VkCommandPool *outCommandPool
                      );

bool allocateCommandBuffer(const VkDevice device,
                           const VkCommandPool commandPool,
                           const VkCommandBufferLevel commandBufferLevel,
                           VkCommandBuffer *outCommandBuffer
                          );

#endif // INSTANCE_H