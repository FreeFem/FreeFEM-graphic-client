#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "Geometry.h"

namespace ffGraph {
namespace Vulkan {

enum PipelinePrimitiveTopology : uint16_t {
    PIPELINE_PRIMITIVE_TOPOLOGY_POINT_LIST = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    PIPELINE_PRIMITIVE_TOPOLOGY_LINE_LIST = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    PIPELINE_PRIMITIVE_TOPOLOGY_LINE_STRIP = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    PIPELINE_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    PIPELINE_PRIMITIVE_TOPOLOGY_COUNT
};

enum PipelinePolygonMode : uint8_t {
    PIPELINE_POLYGON_MODE_FILL = VK_POLYGON_MODE_FILL,
    PIPELINE_POLYGON_MODE_LINE = VK_POLYGON_MODE_LINE,
    PIPELINE_POLYGON_MODE_COUNT
};

struct Pipeline {

    // Creation params
    PipelinePrimitiveTopology Topology;
    PipelinePolygonMode PolygonMode;
    unsigned int LineWidth;

    // Vulkan objects
    VkPipeline Handle;
    VkPipelineLayout Layout;
    static constexpr int ShaderVertexStage = 0;
    static constexpr int ShaderFragmentStage = 1;
    VkShaderModule ShaderModule[2];
};

bool CreatePipeline(GeometryDescriptor CreationInfos, Pipeline& P, VkShaderModule Shaders[2]);
void DestroyPipeline(Pipeline& P);
void DisplayPipelineType(Pipeline p);


}
}

#endif // PIPELINE_H_