#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace ffGraph {
namespace Vulkan {

enum PipelineTopology {
    POINT_LIST = (1 << 0),
    LINE_LIST = (1 << 1),
    TRIANGLE_LIST = (1 << 2)
};

enum PipelinePolygonMode {
    MODE_POINT = (1 << 3),
    MODE_LINE = (1 << 4),
    MODE_FILL = (1 << 5)
};

struct Pipeline {
    uint32_t ID = 0; // Contain infos on how the pipeline was created.
    VkPipeline Handle;
    VkPipelineLayout Layout;
    static constexpr int ShaderVertexStage = 0;
    static constexpr int ShaderFragmentStage = 1;
    VkShaderModule ShaderModule[2];
};

}
}

#endif // PIPELINE_H_