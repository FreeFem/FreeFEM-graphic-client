#ifndef CAMERA_H_
#define CAMERA_H_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace FEM
{

struct CameraUniform {
    glm::mat4 Projection;
    glm::mat4 View;
    glm::mat4 Model;
};

struct Camera {

    float left, right, bottom, top;
    glm::mat4 Projection;
    glm::mat4 View;

    glm::mat4 finalCamera;
};

glm::mat4 setProjectionOrtho(Camera *Cam, float left, float right, float bottom, float top);
glm::mat4 updateCamera(Camera *Cam, glm::vec2 translation);
void computeCamera(Camera *Cam);

} // namespace FEM

#endif // CAMERA_H_