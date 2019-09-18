#include <cstring>
#include "ffImageManager.h"

namespace ffGraph {

Vulkan::ffImage ffImageManager_SearchFor(const ffImageManager& ImageManager, const std::string& Key) {
    Vulkan::ffImage EmptyImage;
    memset(&EmptyImage, 0, sizeof(Vulkan::ffImage));

    for (const auto& ImgHandle : ImageManager) {
        if (Key.compare(ImgHandle.Key) == 0) {
            return ImgHandle.Data;
        }
    }
    return EmptyImage;
}

void ffImageManager_Destroy(ffImageManager& ImageManager, const VkDevice& Device, const VmaAllocator& Allocator) {
    for (auto& ImgHandle : ImageManager) {
        Vulkan::ffDestroyImage(Allocator, Device, ImgHandle.Data);
    }
}

}    // namespace ffGraph