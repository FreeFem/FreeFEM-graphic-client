#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "../util/utils.h"

namespace gr {

    struct GlobalUniformInfos {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

};

#endif // CAMERA_H