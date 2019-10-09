#include <iostream>
#include "Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace ffGraph {
namespace Vulkan {

void UpdateCameraHandle(CameraHandle& Cam) { Cam.ViewProjMatrix = Cam.ProjectionMatrix * Cam.ViewMatrix; }

}    // namespace Vulkan
}    // namespace ffGraph