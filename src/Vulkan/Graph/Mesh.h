#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <glm/mat4x4.hpp>

struct Geometry {
    glm::mat4 Transform;
};

struct Mesh {
    uint16_t RelativePlot;
    glm::mat4 Tranform;

    std::vector<Geometry> Geometries;
};

#endif // MESH_H_