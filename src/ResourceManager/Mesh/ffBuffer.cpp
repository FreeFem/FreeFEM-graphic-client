#include "ffBuffer.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

ffBuffer ffCreateBuffer(VmaAllocator Allocator, ffBufferCreateInfo pCreateInfo,
                        VmaAllocationCreateInfo pAlloctionInfo) {
    VkBufferCreateInfo vkCreateInfo = {};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkCreateInfo.size = pCreateInfo.ElementSize * pCreateInfo.ElementCount;
    vkCreateInfo.usage = pCreateInfo.Usage;
    vkCreateInfo.sharingMode = pCreateInfo.SharingMode;

    ffBuffer n;
    memset(&n, 0, sizeof(ffBuffer));
    if (vmaCreateBuffer(Allocator, &vkCreateInfo, &pAlloctionInfo, &n.Handle, &n.Memory, &n.AllocationInfos)) {
        LogError(GetCurrentLogLocation( ), "Failed to create ffBuffer");
        return n;
    }
    return n;
}

void ffDestroyBuffer(VmaAllocator Allocator, ffBuffer Buffer) {
    vmaDestroyBuffer(Allocator, Buffer.Handle, Buffer.Memory);
    memset(&Buffer, 0, sizeof(ffBuffer));
}

bool ffMapArrayToBuffer(ffBuffer Buffer, Array data) {
    if (!ffIsBufferReady(Buffer)) {
        LogWarning(GetCurrentLogLocation( ), "Trying to map Array to a unready ffBuffer.");
        return false;
    }
    memcpy(Buffer.AllocationInfos.pMappedData, data.Data, Buffer.AllocationInfos.size);
    return true;
}

bool ffMapDataToBuffer(ffBuffer Buffer, void *data) {
    if (!ffIsBufferReady(Buffer)) {
        LogWarning(GetCurrentLogLocation( ), "Trying to map data to a unready ffBuffer.");
        return false;
    }
    memcpy(Buffer.AllocationInfos.pMappedData, data, Buffer.AllocationInfos.size);
    return true;
}

}    // namespace Vulkan
}    // namespace ffGraph
