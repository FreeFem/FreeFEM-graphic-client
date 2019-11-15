#ifndef PLOT_H_
#define PLOT_H_

#include <vector>
#include <cstdint>
#include <glm/mat4x4.hpp>
#include "Mesh.h"

namespace ffGraph {
struct Plot {
    uint16_t PlotID;
    glm::mat4 Transform;

    std::vector<Mesh> Meshes;
};

}

#endif // PLOT_H_