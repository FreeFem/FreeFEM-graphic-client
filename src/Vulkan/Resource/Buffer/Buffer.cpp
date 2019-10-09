#include "Buffer.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

Buffer CreateBuffer(VmaAllocator Allocator, BufferCreateInfo pCreateInfo) {
    if (pCreateInfo.vkData.Size == 0 || pCreateInfo.vkData.SharingMode == VK_SHARING_MODE_MAX_ENUM) {
        LogError(GetCurrentLogLocation( ), "ffGraph::Vulkan::BufferCreateInfo pCreateInfo wasn't filled correctly.");
        return {VK_NULL_HANDLE, VK_NULL_HANDLE, {0, 0, 0, 0, 0, 0}};
    }
    VkBufferCreateInfo BufferInfos = {};
    BufferInfos.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    BufferInfos.size = pCreateInfo.vkData.Size;
    BufferInfos.usage = pCreateInfo.vkData.Usage;
    BufferInfos.sharingMode = pCreateInfo.vkData.SharingMode;
    BufferInfos.queueFamilyIndexCount = pCreateInfo.vkData.queueFamilyIndexCount;
    BufferInfos.pQueueFamilyIndices = pCreateInfo.vkData.pQueueFamilyIndices;

    VmaAllocationCreateInfo AllocInfo = {};
    AllocInfo.usage = pCreateInfo.vmaData.Usage;
    AllocInfo.requiredFlags = pCreateInfo.vmaData.requiredFlags;
    AllocInfo.preferredFlags = pCreateInfo.vmaData.preferredFlags;
    AllocInfo.flags = pCreateInfo.vmaData.flags;

    Buffer n;
    if (vmaCreateBuffer(Allocator, &BufferInfos, &AllocInfo, &n.Handle, &n.Memory, &n.Infos)) {
        LogError(GetCurrentLogLocation( ), "Failed to create Buffer.\n");
        return {VK_NULL_HANDLE, VK_NULL_HANDLE, {0, 0, 0, 0, 0, 0}};
    }
    return n;
}

void DestroyBuffer(VmaAllocator Allocator, Buffer toDestroy) {
    vmaDestroyBuffer(Allocator, toDestroy.Handle, toDestroy.Memory);
}

}    // namespace Vulkan
}    // namespace ffGraph