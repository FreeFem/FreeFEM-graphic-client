#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "../util/NonCopyable.h"
#include "../util/utils.h"

class SwapchainCreator : public NonCopyable {
    public:

        FORCE_USE_RESULT Error init(VkPhysicalDevice physicalDevice, VkDevice device, GLFWwindow *window, VkSurfaceKHR surface);

        FORCE_USE_RESULT Error newSwapchain(VkSwapchainKHR oldSwapchain, VkSwapchainKHR& outSwapchain);

        Error GetSurfaceFormat(VkFormat& surfaceFormat);

        GLFWwindow *GetWindow() { return m_window; }

    private:
        static constexpr VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;
        GLFWwindow *m_window = VK_NULL_HANDLE;
        VkSurfaceKHR m_surface = VK_NULL_HANDLE;
};

#endif // SWAPCHAIN_H