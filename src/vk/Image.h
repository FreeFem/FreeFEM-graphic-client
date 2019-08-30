#ifndef IMAGE_H
#define IMAGE_H

#include <vulkan/vulkan.h>
#include "../util/NonCopyable.h"
#include "../util/utils.h"
#include "vk_mem_alloc.h"

namespace FEM {

namespace gr {

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
     * @param const VkMemoryPropertyFlags memProps[in] - Prefered memory
     * properties.
     * @param VkImageAspectFlags[in] - Image's aspect flag.
     *
     * @return ErrorValues - Returns ErrorValues::NONE if initialization is
     * successful.
     */
    ErrorValues init(const Manager& grm, const VkBufferUsageFlags imageUsage, const VkFormat imageFormat,
                     VkImageAspectFlags viewSubresoucesAspectMask);

    /**
     * @brief Destroy image.
     */
    void destroy(const Manager& grm);

    /**
     * @brief Vulkan image handle getter.
     */
    inline VkImage getImage( ) const { return Handle; }

    /**
     * @brief Vulkan image view handle getter.
     */
    inline VkImageView getImageView( ) const { return View; }

    /**
     * @brief Vulkan image memory handle getter.
     */
    inline VmaAllocation getAllocation( ) const { return Memory; }

   private:
    VkImage Handle = VK_NULL_HANDLE;
    VkImageView View = VK_NULL_HANDLE;
    VmaAllocation Memory;
};

}    // namespace gr

}    // namespace FEM

#endif    // IMAGE_H