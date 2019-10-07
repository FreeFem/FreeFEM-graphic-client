#include <iostream>
#include "Camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace ffGraph {
namespace Vulkan {

static Camera InitCameraInternal(Camera& Cam, float left, float right, float bottom, float top) {
    Cam.Data.ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    Cam.Data.ViewMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.f, -1.f, 1.f));
    Cam.Data.ViewProjectionMatrix = Cam.Data.ProjectionMatrix * Cam.Data.ViewMatrix;
    Cam.Type = _2D;
    return Cam;
}

static void RecalculateViewMatrix(Camera& Cam) {
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), Cam.CameraPosition) *
                          glm::rotate(glm::mat4(1.0f), glm::radians(Cam.CameraRotation), glm::vec3(0, 0, 1));

    Cam.Data.ViewMatrix = glm::scale(glm::inverse(transform), glm::vec3(1.f, -1.f, 1.f));
    Cam.Data.ViewProjectionMatrix = Cam.Data.ProjectionMatrix * Cam.Data.ViewMatrix;
}

void SetProjection(Camera& Cam, float left, float right, float bottom, float top) {
    Cam.Data.ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    Cam.Data.ViewProjectionMatrix = Cam.Data.ProjectionMatrix * Cam.Data.ViewMatrix;
}

Camera InitCamera(float AspectRatio, bool rotation) {
    Camera Cam;

    Cam.AspectRatio = AspectRatio;
    Cam.CanRotate = rotation;
    InitCameraInternal(Cam, -AspectRatio * Cam.ZoomLevel, AspectRatio * Cam.ZoomLevel, -Cam.ZoomLevel, Cam.ZoomLevel);
    return Cam;
}

void CameraSetPosition(Camera& Cam, glm::vec3 nPosition) {
    Cam.CameraPosition = nPosition;
    RecalculateViewMatrix(Cam);
}

void CameraTranslate(Camera& Cam, glm::vec3 Translation) {
    Cam.CameraPosition += Translation;
    RecalculateViewMatrix(Cam);
}

void CameraSetAspectRatio(Camera& Cam, float AspectRatio) {
    Cam.AspectRatio = AspectRatio;
    SetProjection(Cam, -AspectRatio * Cam.ZoomLevel, AspectRatio * Cam.ZoomLevel, -Cam.ZoomLevel, Cam.ZoomLevel);
}
void CameraResetPositionAndZoom(Camera& Cam) {
    Cam.CameraPosition = glm::vec3(0.f, 0.f, 0.f);
    Cam.ZoomLevel = 1.f;
    CameraSetAspectRatio(Cam, Cam.AspectRatio);
    RecalculateViewMatrix(Cam);
}

void ApplyCameraTo2DPosition(Camera& Cam, float x, float y)
{
    glm::vec4 v = glm::vec4(x, y, 0.f, 1.f);

    v = glm::inverse(Cam.Data.ViewProjectionMatrix) * v;
    std::cout << "[Mouse Position in world space] : {" << v.x << ", " << v.y << "}\n";
}

void SwitchCameraType(Camera& Cam)
{
    std::cout << Cam.Type << "\n";
    Cam.Type = (Cam.Type == _2D) ? _3D : _2D;

    if (Cam.Type == _2D) {
        CameraSetAspectRatio(Cam, Cam.AspectRatio);
    } else {
        std::cout << "Camera 3D.\n";
        Cam.Data.ProjectionMatrix = glm::perspective(90.f, Cam.AspectRatio, -1.f, 1.f);
        Cam.Data.ViewMatrix = glm::lookAt(glm::vec3(5.f, 5.f, 5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        Cam.Data.ViewProjectionMatrix = Cam.Data.ProjectionMatrix * Cam.Data.ViewMatrix;
    }
}


}    // namespace Vulkan
}    // namespace ffGraph