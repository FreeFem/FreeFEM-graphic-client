#include <cstring>
#include "ffImageManager.h"

namespace ffGraph {

Vulkan::ffImage ffImageManager_SearchFor(const ffImageManager& Manager, ffHandle Handle) {
    Vulkan::ffImage EmptyImage;
    memset(&EmptyImage, 0, sizeof(Vulkan::ffImage));

    if (Handle.Data.Type == ffHandleType::FF_HANDLE_TYPE_IMAGE)
        return EmptyImage;
    if (Manager[Handle.Data.Cell].UniqueBytes == Handle.Data.UniqueBytes)
        return Manager[Handle.Data.Cell].Data;
    return EmptyImage;
}

void ffImageManager_DestroyItem(ffImageManager& Manager, const VkDevice& Device, const VmaAllocator& Allocator, ffHandle Handle)
{
    if (!Manager[Handle.Data.Cell].isUsed && Manager[Handle.Data.Cell].UniqueBytes != Handle.Data.UniqueBytes)
        return;
    Vulkan::ffDestroyImage(Allocator, Device, Manager[Handle.Data.Cell].Data);
    Manager[Handle.Data.Cell].isUsed = false;
    Manager[Handle.Data.Cell].UniqueBytes += 1;
    memset(&Manager[Handle.Data.Cell].Data, 0, sizeof(Vulkan::ffImage));
}

void ffImageManager_Destroy(ffImageManager& ImageManager, const VkDevice& Device, const VmaAllocator& Allocator) {
    for (auto& ImgHandle : ImageManager) {
        ImgHandle.isUsed = false;
        ImgHandle.UniqueBytes += 1;
        Vulkan::ffDestroyImage(Allocator, Device, ImgHandle.Data);
    }
}

}    // namespace ffGraph