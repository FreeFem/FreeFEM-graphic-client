#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "../util/NonCopyable.h"
#include "../util/utils.h"

namespace gr
{

    class Manager;

    /**
     * @brief Wrapper around a vulkan image.
     */
    class Image {
        public:

            /**
             * @brief Initialize a new vulkan image.
             *
             * @param const Manager& grm[in] - Graphic manager used to create the image.
             * @param const VkBufferUsageFlags imageUsage[in] - Image's use.
             * @param const VkMemoryPropertyFlags memProps[in] - Prefered memory properties.
             * @param VkImageAspectFlags[in] - Image's aspect flag.
             *
             * @return Error - Returns Error::NONE if initialization is successful.
             */
            Error init(const Manager& grm, const VkBufferUsageFlags imageUsage,
                    const VkMemoryPropertyFlags requiredMemProps, const VkFormat imageFormat,
                    VkImageAspectFlags viewSubresoucesAspectMask);

            /**
             * @brief Destroy image.
             */
            void destroy(const Manager& grm);

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
            inline VmaAllocation getAllocation() const { return m_memory; }


        private:
            VkImage m_handle = VK_NULL_HANDLE;
            VkImageView m_view = VK_NULL_HANDLE;
            VmaAllocation m_memory;
    };

} // namespace gr

#endif // IMAGE_H