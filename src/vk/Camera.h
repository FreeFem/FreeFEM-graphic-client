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

    class Camera {
        public:

            void init(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), float yaw = -90.f, float pitch = 0.f)
            {
                MovementSpeed = 2.5f;
                Front = front;
                Position = position;
                WorldUp = up;
                Yaw = yaw;
                Pitch = pitch;
                updateCameraVectors();
            }

            inline glm::mat4 *getCameraMatrix() { matrix = glm::lookAt(Position, Position + Front, Up); return &matrix; }

            void updateCameraVectors()
            {
                // Calculate the new Front vector
                glm::vec3 front;
                front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
                front.y = sin(glm::radians(Pitch));
                front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
                Front = glm::normalize(front);
                // Also re-calculate the Right and Up vector
                Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
                Up    = glm::normalize(glm::cross(Right, Front));
            }

        private:
            glm::vec3 Position;
            glm::vec3 Front;
            glm::vec3 Up;
            glm::vec3 Right;
            glm::vec3 WorldUp;
            glm::mat4x4 matrix;

            float Yaw;
            float Pitch;

            float MovementSpeed;

    };

};

#endif // CAMERA_H