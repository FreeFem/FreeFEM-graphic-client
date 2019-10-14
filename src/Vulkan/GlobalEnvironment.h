#ifndef GLOBAL_ENVIRONMENT_H_
#define GLOBAL_ENVIRONMENT_H_

#include "Environment.h"

static ffGraph::Vulkan::Environment *GlobalEnvironmentPTR = 0;

inline VkDevice GetDevice( ) { return GlobalEnvironmentPTR->GPUInfos.Device; }
inline VkPhysicalDevice GetPhysicalDevice( ) { return GlobalEnvironmentPTR->GPUInfos.PhysicalDevice; }
inline VkSurfaceKHR GetSurface( ) { return GlobalEnvironmentPTR->ScreenInfos.Surface; }

#endif    // GLOBAL_ENVIRONMENT_H_