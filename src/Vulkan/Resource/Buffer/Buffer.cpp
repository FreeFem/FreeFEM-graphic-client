#include "Buffer.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

Buffer CreateBuffer(VmaAllocator Allocator, BufferCreateInfo pCreateInfo)
{
    if (pCreateInfo.vkData.Size == 0 || pCreateInfo.vkData.Usage == VK_BUFFER_USAGE_FLAGS_BITS_MAX_ENUM || pCreateInfo.vkData.SharingMode == VK_SHARING_MODE_MAX_ENUM) {
        LogError(GetCurrentLogLocation(), "ffGraph::Vulkan::BufferCreateInfo pCreateInfo wasn't filled correctly.");
        return {VK_NULL_HANDLE, VK_NULL_HANDLE, {0}};
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

}


} // namespace Vulkan
} // namespace ffGraph