#ifndef FF_CONTEXT_H_
#define FF_CONTEXT_H_

#include <vulkan/vulkan.h>
#include <utility>
#include "Window/NativeWindow.h"
#include "ffInstance.h"
#include "ffDevice.h"

namespace ffGraph {
namespace Vulkan {

struct Context {
    NativeWindow Window;
    ffInstance Instance;
    VkSurfaceKHR Surface;
    ffDevice Device;
};

bool newContext(Context *vkContext);

void destroyContext(Context vkContext);

} // namespace Vulkan
} // namespace ffGraph


#endif // FF_CONTEXT_H_