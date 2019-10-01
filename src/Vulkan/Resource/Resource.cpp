#include "Resource.h"

namespace ffGraph {
namespace Vulkan {

Resource NewResources(const VkDevice& Device)
{
    Resource n;

    n.GeometryVertex = CreateShader("./shaders/geometry.vert.spirv", Device, VK_SHADER_STAGE_VERTEX_BIT);
    n.GeometryFragment = CreateShader("./shaders/geometry.frag.spirv", Device, VK_SHADER_STAGE_FRAGMENT_BIT);

    return n;
}

void DestroyResources(const VkDevice& Device, Resource& Resources)
{
    DestroyShader(Device, Resources.GeometryVertex);
    DestroyShader(Device, Resources.GeometryFragment);
}

}
}