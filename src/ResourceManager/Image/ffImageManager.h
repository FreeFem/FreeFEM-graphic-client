#ifndef FF_IMAGE_MANAGER_H_
#define FF_IMAGE_MANAGER_H_

#include <vector>
#include <string>
#include "ffImage.h"

namespace ffGraph {

struct ffImageHandle {
    std::string Key;
    Vulkan::ffImage Data;
};

typedef std::vector<ffImageHandle> ffImageManager;

Vulkan::ffImage ffImageManager_SearchFor(const ffImageManager& ImageManager, const std::string& Key);

void ffImageManager_Destroy(ffImageManager& ImageManager, const VkDevice& Device, const VmaAllocator& Allocator);

Vulkan::ffImage ffImageManager_NewImage(const VmaAllocator& Allocator, const VkDevice& Device, ffImageManager& Manager, const std::string Key, Vulkan::ffImageCreateInfo pCreateInfo, VmaAllocationCreateInfo pAllocationInfos);

} // namespace ffGraph


#endif // FF_IMAGE_MANAGER_H_