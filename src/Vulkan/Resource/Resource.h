#ifndef RESOURCE_H_
#define RESOURCE_H_

#include "Shader/Shader.h"

namespace ffGraph {
namespace Vulkan {

struct Resource {
    Shader GeometryVertex;
    Shader GeometryFragment;
};

Resource NewResources(const VkDevice& Device);

void DestroyResources(const VkDevice& Device, Resource& Resources);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // RESOURCE_H_