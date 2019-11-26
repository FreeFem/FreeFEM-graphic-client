#ifndef N_PIPELINE_H_
#define N_PIPELINE_H_

#include <cstring>
#include "Resource/Shader.h"
#include "../../ffTypes.h"
#include "Descriptor.h"

namespace ffGraph {
namespace Vulkan {

struct PushConstant {
    VkDeviceSize Size = 0;
    VkShaderStageFlags Stage;
    void *pData = NULL;
};

struct PipelineShaderInfo {
    VkShaderStageFlagBits Stage;
    VkShaderModule Module;
};

struct PipelineDataFormat {
    VkFormat Format;
    VkDeviceSize Offset;
};

struct PipelineCreateInfos {
    PushConstant PushConstantHandle;
    DescriptorHandle DescriptorListHandle;
    std::vector<PipelineShaderInfo> ShaderInfos;
    VkDeviceSize VertexSize;
    std::vector<PipelineDataFormat> VertexFormat;

    VkPrimitiveTopology Topology;
    VkPolygonMode PolygonMode;
    uint32_t LineWidth;

    PipelineCreateInfos operator=(PipelineCreateInfos& copy) {
        memcpy(&PushConstantHandle, &copy.PushConstantHandle, sizeof(PushConstant));

        DescriptorListHandle.ffType = copy.DescriptorListHandle.ffType;
        DescriptorListHandle.List.reserve(copy.DescriptorListHandle.List.size());
        DescriptorListHandle.List.insert(copy.DescriptorListHandle.List.begin(), copy.DescriptorListHandle.List.begin(), copy.DescriptorListHandle.List.end());

        ShaderInfos.reserve(copy.ShaderInfos.size());
        ShaderInfos.insert(copy.ShaderInfos.begin(), copy.ShaderInfos.begin(), copy.ShaderInfos.end());

        VertexSize = copy.VertexSize;
        VertexFormat.reserve(copy.VertexFormat.size());
        VertexFormat.insert(copy.VertexFormat.begin(), copy.VertexFormat.begin(), copy.VertexFormat.end());

        Topology = copy.Topology;
        PolygonMode = copy.PolygonMode;
        LineWidth = copy.LineWidth;

        return *this;
    };
};

struct Pipeline {
    PipelineCreateInfos CreationData;

    VkPipeline Handle;
    VkPipelineLayout Layout;

    VkDescriptorPool DescriptorPool;
    VkDescriptorSetLayout DescriptorLayout;
    VkDescriptorSet DescriptorSet;
};

PipelineCreateInfos GetPipelineCreateInfos(ffTypes type, ShaderLibrary& ShaderLib, void *pPushConstantData, size_t PushConstantSize, VkShaderStageFlags PushConstantStage);
bool ConstructPipeline(Pipeline& P, PipelineCreateInfos& CreateInfo);
void DestroyPipeline(Pipeline& p);

} // namespace Vulkan
} // namespace ffGraph

#endif // N_PIPELINE_H_