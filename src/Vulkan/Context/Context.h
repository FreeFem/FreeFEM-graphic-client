#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <vulkan/vulkan.h>
#include <utility>
#include "../Window/NativeWindow.h"
#include "Device.h"
#include "Swapchain.h"

namespace ffGraph {
namespace Vulkan {

struct Context {
    VkSurfaceKHR Surface;
    Device vkDevice;
    VkSurfaceFormatKHR SurfaceFormat;
    Swapchain vkSwapchain;
};

bool newContext(Context& vkContext, const VkInstance& Instance, const NativeWindow& Window);

void destroyContext(Context vkContext, const VkInstance& Instance);

} // namespace Vulkan
} // namespace ffGraph


#endif // CONTEXT_H_