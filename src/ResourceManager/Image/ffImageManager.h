/**
 * @file ffImageManager.h
 * @brief Declaration of ffImageManager data type and it's related functions.
 */
#ifndef FF_IMAGE_MANAGER_H_
#define FF_IMAGE_MANAGER_H_

#include <array>
#include <string>
#include "ffHandle.h"
#include "ffImage.h"

namespace ffGraph {

/**
 * @brief A ffGraph::Vulkan::ffImage with it's key.
 */
struct ffImageHandle {
    bool isUsed = false;
    uint16_t UniqueBytes = 0;
    Vulkan::ffImage Data;
};

// @brief Store all ffGraph::Vulkan::ffImage with it's key
typedef std::array<ffImageHandle, 32> ffImageManager;

/**
 * @brief Search a ffGraph::Vulkan::ffImage in the ffGraph::ffImageManager.
 *
 * @param ImageManager [in] - ffGraph::ffImageManager to search in.
 * @param Key [in] - Key associated with the ffGraph::Vulkan::ffImage searched.
 *
 * @return ffGraph::Vulkan::ffImage - Use ffGraph::Vulkan::ffIsImageReady() to check return value.
 */
Vulkan::ffImage ffImageManager_SearchFor(const ffImageManager& ImageManager, ffHandle Index);

/**
 * @brief destroy the ffGraph::ffImageManager, destroying all the ffGraph::Vulkan::ffImage it own.
 *
 * @param ImageManger [in] - ffGraph::ffImageManager to destroy.
 * @param Device [in] - VkDevice used to create an VkImageView.
 * @param Allocator [in] - VmaAllocator used to allocate the VkImage memory.
 *
 * @return void
 */
void ffImageManager_Destroy(ffImageManager& ImageManager, const VkDevice& Device, const VmaAllocator& Allocator);

}    // namespace ffGraph

#endif    // FF_IMAGE_MANAGER_H_