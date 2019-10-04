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

struct Camera {
    struct {
        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewMatrix;

        glm::mat4 ViewProjectionMatrix;
    } Data;
    float AspectRatio;
    float ZoomLevel;
    bool CanRotate;

    glm::vec3 CameraPosition;
    float CameraRotation = 0.f;
    float CameraTranslationSpeed = 1.f;
    float CameraRotationSpeed = 1.f;
};

Camera InitCamera(float AspectRatio, bool rotation);
void CameraSetPosition(Camera& Cam, glm::vec3 nPosition);
void CameraTranslate(Camera& Cam, glm::vec3 Translation);
void SetProjection(Camera& Cam, float left, float right, float bottom, float top);
void CameraSetAspectRatio(Camera& Cam, float AspectRatio);
void CameraResetPositionAndZoom(Camera& Cam);
void ApplyCameraTo2DPosition(Camera& Cam, float x, float y);
}    // namespace Vulkan
}    // namespace ffGraph

#endif    // CAMERA_H_