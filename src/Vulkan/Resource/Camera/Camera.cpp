#include "Camera.h"

namespace ffGraph {
namespace Vulkan {

static Camera InitCameraInternal(Camera& Cam, float left, float right, float bottom, float top)
{
    Cam.Data.ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    Cam.Data.ViewMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.f, -1.f, 1.f));
    Cam.Data.ViewProjectionMatrix = Cam.Data.ProjectionMatrix * Cam.Data.ViewMatrix;
    return Cam;
}

static void RecalculateViewMatrix(Camera& Cam)
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), Cam.CameraPosition) * glm::rotate(glm::mat4(1.0f), glm::radians(Cam.CameraRotation), glm::vec3(0, 0, 1));

    Cam.Data.ViewMatrix = glm::scale(glm::inverse(transform), glm::vec3(1.f, -1.f, 1.f));
    Cam.Data.ViewProjectionMatrix = Cam.Data.ProjectionMatrix * Cam.Data.ViewMatrix;
}

void SetProjection(Camera& Cam, float left, float right, float bottom, float top)
{
    Cam.Data.ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.f, 1.f);
    Cam.Data.ViewProjectionMatrix = Cam.Data.ProjectionMatrix * Cam.Data.ViewMatrix;
}

Camera InitCamera(float AspectRatio, bool rotation)
{
    Camera Cam;

    Cam.AspectRatio = AspectRatio;
    Cam.CanRotate = rotation;
    InitCameraInternal(Cam, -AspectRatio * Cam.ZoomLevel, AspectRatio * Cam.ZoomLevel, -Cam.ZoomLevel, Cam.ZoomLevel);
    return Cam;
}

void CameraSetPosition(Camera& Cam, glm::vec3 nPosition)
{
    Cam.CameraPosition = nPosition;
    RecalculateViewMatrix(Cam);
}

void CameraTranslate(Camera& Cam, glm::vec3 Translation)
{
    Cam.CameraPosition += Translation;
    RecalculateViewMatrix(Cam);
}

void CameraSetAspectRatio(Camera& Cam, float AspectRatio)
{
    Cam.AspectRatio = AspectRatio;
    SetProjection(Cam, -AspectRatio * Cam.ZoomLevel, AspectRatio * Cam.ZoomLevel, -Cam.ZoomLevel, Cam.ZoomLevel);
}
void CameraResetPositionAndZoom(Camera& Cam)
{
    Cam.CameraPosition = glm::vec3(0.f, 0.f, 0.f);
    Cam.ZoomLevel = 1.f;
    CameraSetAspectRatio(Cam, Cam.AspectRatio);
    RecalculateViewMatrix(Cam);
}
}
}