#include <vector>
#include <string>
#include <algorithm>
#include "ffContext.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

bool ffnewContext(ffContext *vkContext)
{
    if (ffInitGFLW() != true)
        return false;
    vkContext->Window = ffNewWindow({1290, 768});
    if (vkContext->Window.Handle == 0)
        return false;
    vkContext->Instance = ffNewInstance(vkContext->Window.SurfaceExtensions, {});
    if (vkContext->Instance.Handle == VK_NULL_HANDLE)
        return false;
    vkContext->Surface = ffGetSurface(vkContext->Instance.Handle, vkContext->Window);
    if (vkContext->Surface == VK_NULL_HANDLE)
        return false;
    vkContext->Device = ffNewDevice(vkContext->Instance.Handle, vkContext->Surface, {});
    if (vkContext->Device.Handle == VK_NULL_HANDLE || vkContext->Device.PhysicalHandle == VK_NULL_HANDLE)
        return false;
    return true;
}

} // namespace Vulkan
} // namespace ffGraph