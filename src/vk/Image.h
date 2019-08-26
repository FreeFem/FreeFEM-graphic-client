#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan/vulkan.h>
#include "../util/NonCopyable.h"
#include "../util/utils.h"

namespace gr
{

    /**
     * @brief Wrapper around a vulkan image.
     */
    class Image {
        public:

            /**
             * @brief Initialize a new vulkan image.
             *
             * @param const VkDevice device[in] - Device used to allocate the memory.
             * @param const VkPhysicalDeviceMemoryProperties memPhysicalDeviceProps[in] - Physical device memory properties.
             * @param const VkBufferUsageFlags imageUsage[in] - Image's use.
             * @param const VkMemoryPropertyFlags memProps[in] - Prefered memory properties.
             * @param const int width[in] - Image's width in pixel.
             * @param const int height[in] - Image's height in pixel.
             * @param VkImageAspectFlags[in] - Image's aspect flag.
             *
             * @return Error - Returns Error::NONE if initialization is successful.
             */
            Error init(const VkDevice, const VkPhysicalDeviceMemoryProperties, const VkBufferUsageFlags,
                        const VkMemoryPropertyFlags, const VkFormat imageFormat, const int width, const int height,
                        VkImageAspectFlags viewSubresoucesAspectMask);

            /**
             * @brief Destroy image.
             */
            void destroy(const VkDevice& device);

            /**
             * @brief Vulkan image handle getter.
             */
            inline VkImage getImage() const { return m_handle; }

            /**
             * @brief Vulkan image view handle getter.
             */
            inline VkImageView getImageView() const { return m_view; }

            /**
             * @brief Vulkan image memory handle getter.
             */
            inline VkDeviceMemory getMemory() const { return m_memory; }


        private:
            VkImage m_handle = VK_NULL_HANDLE;
            VkImageView m_view = VK_NULL_HANDLE;
            VkDeviceMemory m_memory = VK_NULL_HANDLE;
    };

} // namespace gr

#endif // IMAGE_H