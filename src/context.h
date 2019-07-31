#include <volk.h>

struct VulkanContext {
    VkInstance Instance;
    VkPhysicalDevice PhysicalDevice;
    uint32_t FamilyIndex;
    VkDevice Device;
    VkQueue Queue;
};

/**
 * @brief Fill a VulkanContext
 *
 * @param VulkanContext *context - Context to fill with data
 */
bool createVulkanContext(VulkanContext *context);

/**
 * @breif Destroy a VulkanContext
 *
 * @param VulkanContext context - Context to destroy
 */
void destroyVulkanContext(VulkanContext context);