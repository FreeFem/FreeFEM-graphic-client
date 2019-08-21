#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan/vulkan.h>
#include "../util/NonCopyable.h"
#include "../util/utils.h"

class Image : public NonCopyable {
    public:

        Error init(const VkDevice, const VkPhysicalDeviceMemoryProperties, const VkBufferUsageFlags,
                    const VkMemoryPropertyFlags, const VkFormat imageFormat, const int width, const int height,
                    VkImageAspectFlags viewSubresoucesAspectMask);

        void destroy();

        VkImage m_handle;
        VkImageView m_view;
        VkDeviceMemory m_memory;
};

#endif // IMAGE_H