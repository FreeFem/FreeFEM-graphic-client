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
    bool ShowParameterWindow = false;
    bool PolygonModeState = false;
};

struct Mesh {
    uint16_t MeshID;
    glm::mat4 Tranform;
    bool Selected = false;

    std::vector<GeoUiData> UiInfos;
    std::vector<Geometry> Geometries;
};

}

#endif // MESH_H_