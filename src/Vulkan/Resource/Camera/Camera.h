#ifndef CAMERA_H_
#define CAMERA_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace ffGraph {
namespace Vulkan {

struct CameraUniform {
    glm::mat4 ViewProj;
    glm::mat4 Model;
};

struct CameraHandle {
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewProjMatrix;
};

void UpdateCameraHandle(CameraHandle& Cam);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // CAMERA_H_