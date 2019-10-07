#include <GLFW/glfw3.h>
#include <algorithm>
#include "CameraController.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static void InitCameraControllerInternal(CameraController& Cam, CameraType Type)
{
    Cam.Handle.ProjectionMatrix = glm::ortho(-Cam.AspectRatio * Cam.ZoomLevel, Cam.AspectRatio * Cam.ZoomLevel, -Cam.ZoomLevel, Cam.ZoomLevel, -100.f, 100.f);
    if (Type == CameraType::_2D) {
        Cam.Handle.ViewMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.f, -1.f, 1.f));
    } else {
        Cam.Handle.ViewMatrix = glm::scale(glm::lookAt(Cam.Position, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)), glm::vec3(1.f, -1.f, 1.f));
    }
    UpdateCameraHandle(Cam.Handle);
}

static void UpdateCameraControllerProjection(CameraController& Cam)
{
    Cam.Handle.ProjectionMatrix = glm::ortho(-Cam.AspectRatio * Cam.ZoomLevel, Cam.AspectRatio * Cam.ZoomLevel, -Cam.ZoomLevel, Cam.ZoomLevel, -100.f, 100.f);
    UpdateCameraHandle(Cam.Handle);
}

void InitCameraController(CameraController& Cam, float AspectRatio, float FOV, CameraType BaseType)
{
    Cam.FOV = FOV;
    Cam.AspectRatio = AspectRatio;
    Cam.Position = glm::vec3(0.f, 0.f, 0.f);
    Cam.Rotation = glm::vec3(0.f, 0.f, 0.f);
    Cam.ZoomLevel = 1.f;

    Cam.Type = BaseType;
    InitCameraControllerInternal(Cam, BaseType);
}

void CameraKeyEvents(CameraController& Cam, int key)
{
    if (key == GLFW_KEY_UP) {
        Cam.Translate(glm::vec3(0.f, -0.5f, 0.f));
    } else if (key == GLFW_KEY_DOWN) {
        Cam.Translate(glm::vec3(0.f, 0.5f, 0.f));
    } else if (key == GLFW_KEY_LEFT) {
        Cam.Translate(glm::vec3(-0.5f, 0.f, 0.f));
    } else if (key == GLFW_KEY_RIGHT) {
        Cam.Translate(glm::vec3(0.5f, 0.f, 0.f));
    } else if (key == GLFW_KEY_KP_3) {
        Cam.Type = !Cam.Type;
        Cam.SetPosition(glm::vec3(5.f, 5.f, 5.f));
        Cam.UpdateCameraViewMatrix();
    }
    UpdateCameraHandle(Cam.Handle);
}

void CameraScroolEvents(CameraController& Cam, double yOffset)
{
    Cam.ZoomLevel -= yOffset * 0.25f;
    Cam.ZoomLevel = std::max(Cam.ZoomLevel, 0.25f);
    Cam.ZoomLevel = std::min(Cam.ZoomLevel, 5.0f);
    UpdateCameraControllerProjection(Cam);
}

}
}