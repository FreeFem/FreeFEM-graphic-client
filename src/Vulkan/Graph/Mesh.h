#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <glm/mat4x4.hpp>
#include "Geometry.h"

namespace ffGraph {

struct GeoUiData {
    glm::mat4 Transform = glm::mat4(1.0f);
    bool Selected = false;
    bool Render = true;
};
struct Mesh {
    uint16_t MeshID;
    glm::mat4 Tranform;

    std::vector<GeoUiData> UiInfos;
    std::vector<Geometry> Geometries;
};

}

#endif // MESH_H_