#ifndef CAMERA_H
#define CAMERA_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include "../util/utils.h"

namespace FEM {

namespace gr {

class Camera {
   public:
    enum CamType { lookAt, firstPerson };

    CamType type = CamType::lookAt;

    glm::vec3 rotation = glm::vec3( );
    glm::vec3 position = glm::vec3( );

    bool updated = false;

    void setPerspective(float field_of_view, float aspect, float near, float far) {
        fov = field_of_view;
        znear = near;
        zfar = far;
        CamHandle.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
    }

    void updateAspectRatio(float aspect) {
        CamHandle.perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
    }

    void setPosition(glm::vec3 new_position) {
        position = new_position;
        updateViewMatrix( );
    }
    void setRotation(glm::vec3 new_rotation) {
        rotation = new_rotation;
        updateViewMatrix( );
    }

    void rotate(glm::vec3 delta) {
        rotation += delta;
        updateViewMatrix( );
    }
    void translate(glm::vec3 delta) {
        position += delta;
        updateViewMatrix( );
    }

    void update(float deltaTime) { updated = false; }

    typedef struct {
        alignas(16) glm::mat4 perspective;
        alignas(16) glm::mat4 view;
        alignas(16) glm::mat4 model = glm::mat4(1.0f);
    } UniformCamera;

    void viewMatrixtest( ) {
        glm::mat4 view = glm::lookAt(glm::vec3(3.f, 3.f, 3.f), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
        CamHandle.view = CamHandle.perspective * view;
    }

    UniformCamera *getCamHandlePTR( ) { return &CamHandle; }

    UniformCamera CamHandle;

   private:
    float fov;
    float znear, zfar;

    void updateViewMatrix( ) {
        glm::mat4 rotM = glm::mat4(1.0f);
        glm::mat4 transM;

        rotM = glm::rotate(rotM, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        rotM = glm::rotate(rotM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotM = glm::rotate(rotM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        transM = glm::translate(glm::mat4(1.0f), position);

        if (type == CamType::firstPerson) {
            CamHandle.view = rotM * transM;
        } else {
            CamHandle.view = glm::lookAt(position, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
        }

        updated = true;
    }
};
};    // namespace gr
};    // namespace FEM

#endif    // CAMERA_H