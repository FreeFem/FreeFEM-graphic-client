#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan/vulkan.h>
#include "../util/NonCopyable.h"
#include "../util/utils.h"

namespace gr
{

    class Image {
        public:

            Error init(const VkDevice, const VkPhysicalDeviceMemoryProperties, const VkBufferUsageFlags,
                        const VkMemoryPropertyFlags, const VkFormat imageFormat, const int width, const int height,
                        VkImageAspectFlags viewSubresoucesAspectMask);

            void destroy(const VkDevice& device);

            inline VkImage getImage() const { return m_handle; }
            inline VkImageView getImageView() const { return m_view; }
            inline VkDeviceMemory getMemory() const { return m_memory; }


        private:
            VkImage m_handle = VK_NULL_HANDLE;
            VkImageView m_view = VK_NULL_HANDLE;
            VkDeviceMemory m_memory = VK_NULL_HANDLE;
    };

} // namespace gr

#endif // IMAGE_H