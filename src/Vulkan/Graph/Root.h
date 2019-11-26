#ifndef ROOT_H_
#define ROOT_H_

#include <vector>
#include <cstdint>
#include <glm/mat4x4.hpp>
#include "Plot.h"
#include "Pipeline.h"
#include "Geometry.h"
#include "Resource/Buffer/Buffer.h"
#include "Resource/Camera/CameraController.h"

namespace ffGraph {
namespace Vulkan {

struct Root {
    glm::mat4 Transform;
    bool Update = true;

    bool UpdateExecutionData = false;
    std::vector<Pipeline> Pipelines;
    // std::vector<Plot> Plots;
    std::vector<ConstructedGeometry> Geometries;
    std::vector<size_t> RenderedGeometries;
    Buffer RenderBuffer;
    CameraController Cam;
    CameraUniform CamUniform;
};

void AddToGraph(Root& r, ConstructedGeometry& g, ShaderLibrary& ShaderLib);
// void GraphTraversal(Root r);
// void ConstructCurrentGraphPipelines(Root& r, VkShaderModule Shaders[2]);
void DestroyGraph(Root& r);

}
}

#endif // ROOT_H_