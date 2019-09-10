#include "Camera.h"

namespace FEM {

glm::mat4 setProjectionOrtho(Camera *Cam, float left, float right, float bottom, float top) {
    Cam->left = left;
    Cam->right = right;
    Cam->bottom = bottom;
    Cam->top = top;
    Cam->Projection = glm::ortho(left, right, bottom, top);
    Cam->View = glm::mat4(1.0f);
    Cam->finalCamera = glm::mat4(1.0f);
    return Cam->Projection;
}

glm::mat4 updateCamera(Camera *Cam, glm::vec2 translation) {
    Cam->left -= translation.x;
    Cam->right += translation.x;

    Cam->bottom -= translation.y;
    Cam->top += translation.x;

    Cam->Projection = glm::ortho(Cam->left, Cam->right, Cam->bottom, Cam->top);

    return Cam->Projection;
}

void computeCamera(Camera *Cam) {
    Cam->finalCamera = Cam->Projection * Cam->View;
    Cam->finalCamera = glm::scale(Cam->finalCamera, glm::vec3(1, -1, 1));
}

}    // namespace FEM