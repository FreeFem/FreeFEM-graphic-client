#ifndef FF_RENDERPASS_H_
#define FF_RENDERPASS_H_

#include <vulkan/vulkan.h>

namespace ffGraph {
namespace Vulkan {

struct ffRenderPass {
    VkRenderPass Handle;
};

} // namespace Vulkan
} // namespace ffGraph


#endif // FF_RENDERPASS_H_