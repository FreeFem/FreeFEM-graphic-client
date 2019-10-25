#ifndef GLOBAL_ENVIRONMENT_H_
#define GLOBAL_ENVIRONMENT_H_

#include "Environment.h"

namespace ffGraph {
namespace Vulkan {

extern Environment *GlobalEnvironmentPTR;

inline VkDevice GetLogicalDevice( ) { return GlobalEnvironmentPTR->GPUInfos.Device; }
inline VkPhysicalDevice GetPhysicalDevice( ) { return GlobalEnvironmentPTR->GPUInfos.PhysicalDevice; }
inline VkQueue GetGraphicQueue( ) {
    return GlobalEnvironmentPTR->GPUInfos.Queues[GlobalEnvironmentPTR->GPUInfos.GraphicQueueIndex];
}
inline VkQueue GetPresentQueue( ) {
    return GlobalEnvironmentPTR->GPUInfos.Queues[GlobalEnvironmentPTR->GPUInfos.PresentQueueIndex];
}
inline VkQueue GetTransferQueue( ) {
    return GlobalEnvironmentPTR->GPUInfos.Queues[GlobalEnvironmentPTR->GPUInfos.TransferQueueIndex];
}
inline VkCommandPool GetCommandPool( ) { return GlobalEnvironmentPTR->GraphManager.CommandPool; }
inline VmaAllocator GetAllocator( ) { return GlobalEnvironmentPTR->Allocator; }
inline VkSampleCountFlagBits GetSampleCount( ) { return GlobalEnvironmentPTR->GPUInfos.Capabilities.msaaSamples; }
inline VkCommandPool GetTransferCommandPool( ) { return GlobalEnvironmentPTR->TransfertCommandPool; }
inline uint32_t GetTransferQueueIndex( ) {
    return GlobalEnvironmentPTR->GPUInfos.QueueIndex[GlobalEnvironmentPTR->GPUInfos.TransferQueueIndex];
}
inline uint32_t GetGraphicQueueIndex( ) {
    return GlobalEnvironmentPTR->GPUInfos.QueueIndex[GlobalEnvironmentPTR->GPUInfos.GraphicQueueIndex];
}

inline VkRenderPass GetRenderPass( ) {
    return GlobalEnvironmentPTR->GraphManager.RenderPass;
}

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // GLOBAL_ENVIRONMENT_H_