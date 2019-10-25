#ifndef ROOT_H_
#define ROOT_H_

#include <vector>
#include <cstdint>
#include <glm/mat4x4.hpp>
#include "Plot.h"
#include "Pipeline.h"

namespace ffGraph {
namespace Vulkan {

struct PipelineExecData {
    uint32_t PipielineID;

    std::vector<Geometry> Geometries;
};

struct Root {
    glm::mat4 Transform;

    bool UpdateExecutionData = false;
    std::vector<Pipeline> Pipelines;
    std::vector<PipelineExecData> ExecData;
    std::vector<Plot> Plots;
};

}
}

#endif // ROOT_H_