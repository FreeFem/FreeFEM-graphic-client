#ifndef CAMERA_CONTROLLER_H_
#define CAMERA_CONTROLLER_H_

#include "Camera.h"

namespace ffGraph {
namespace Vulkan {

enum CameraType {
    // General Type
    _2D = 0,
    _3D = 1
};

struct CameraController {
    uint8_t Type;

    glm::vec3 Position;
    glm::vec3 Rotation;

    float FOV;
    float AspectRatio;
    float ZoomLevel;

    CameraHandle Handle;

    void UpdateCameraViewMatrix( ) {
        // if (Type == CameraType::_2D) {
            glm::mat4 transform = glm::translate(glm::mat4(1.0f), Position) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.x), glm::vec3(1, 0, 0)) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.y), glm::vec3(0, 1, 0)) *
                                  glm::rotate(glm::mat4(1.0f), glm::radians(Rotation.z), glm::vec3(0, 0, 1));

            Handle.ViewMatrix = glm::scale(glm::inverse(transform), glm::vec3(1.f, -1.f, 1.f));
        // } else {
        //     Handle.ViewMatrix = glm::scale(glm::lookAt(Position, glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)),
        //                                    glm::vec3(1.f, -1.f, 1.f));
        // }
        UpdateCameraHandle(Handle);
    }

    inline void Translate(glm::vec3 translation) {
        Position += translation;
        UpdateCameraViewMatrix( );
    }
    inline void Rotate(glm::vec3 rotation) {
        Rotation += rotation;
        UpdateCameraViewMatrix( );
    }
    inline void SetPosition(glm::vec3 nPosition) {
        Position = nPosition;
        UpdateCameraViewMatrix( );
    }
    inline void SetRotation(glm::vec3 nRotation) {
        Rotation = nRotation;
        UpdateCameraViewMatrix( );
    }
};

void InitCameraController(CameraController& Cam, float AspectRatio, float FOV, CameraType BaseType);

void CameraKeyEvents(CameraController& Cam, int key);

void CameraScroolEvents(CameraController& Cam, double yOffset);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // CAMERA_CONTROLLER_H_