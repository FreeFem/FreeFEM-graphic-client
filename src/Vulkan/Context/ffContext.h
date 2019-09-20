#ifndef FF_CONTEXT_H_
#define FF_CONTEXT_H_

#include <vulkan/vulkan.h>
#include <utility>
#include "Window/NativeWindow.h"
#include "ffInstance.h"
#include "ffDevice.h"

namespace ffGraph {
namespace Vulkan {

struct ffContext {
    NativeWindow Window;
    ffInstance Instance;
    VkSurfaceKHR Surface;
    ffDevice Device;
};

bool ffnewContext(ffContext *vkContext);

void ffdestroyContext(ffContext vkContext);

} // namespace Vulkan
} // namespace ffGraph


#endif // FF_CONTEXT_H_