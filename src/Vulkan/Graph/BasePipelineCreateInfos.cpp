#include <iostream>
#include "Pipeline.h"

namespace ffGraph {
namespace Vulkan {

static PipelineCreateInfos NewCurve2DPipeline(ShaderLibrary& ShaderLib, void *pPushConstantData, size_t PushConstantSize, VkShaderStageFlags PushConstantStage)
{
    PipelineCreateInfos n;

    n.Topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    n.PolygonMode = VK_POLYGON_MODE_LINE;
    n.LineWidth = 2;

    n.VertexSize = sizeof(float) * 7;
    n.VertexFormat.resize(2);
    n.VertexFormat[0].Format = VK_FORMAT_R32G32B32_SFLOAT;
    n.VertexFormat[0].Offset = 0;

    n.VertexFormat[1].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
    n.VertexFormat[1].Offset = sizeof(float) * 3;

    n.ShaderInfos.resize(2);
    n.ShaderInfos[0].Stage = VK_SHADER_STAGE_VERTEX_BIT;
    n.ShaderInfos[0].Module = FindShader(ShaderLib, "Geo3D.vert");

    n.ShaderInfos[1].Stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    n.ShaderInfos[1].Module = FindShader(ShaderLib, "Color.frag");

    n.PushConstantHandle.pData = pPushConstantData;
    n.PushConstantHandle.Size = PushConstantSize;
    n.PushConstantHandle.Stage = PushConstantStage;

    n.DescriptorListHandle.ffType = ffTypes::FF_TYPE_CURVE_2D;

    return n;
}

static PipelineCreateInfos NewCurve3DPipeline(ShaderLibrary& ShaderLib, void *pPushConstantData, size_t PushConstantSize, VkShaderStageFlags PushConstantStage)
{
    PipelineCreateInfos n;

    n.Topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    n.PolygonMode = VK_POLYGON_MODE_LINE;
    n.LineWidth = 2;

    n.VertexSize = sizeof(float) * 7;
    n.VertexFormat.resize(2);
    n.VertexFormat[0].Offset = 0;

    n.ShaderInfos.resize(2);
    n.ShaderInfos[0].Stage = VK_SHADER_STAGE_VERTEX_BIT;
    n.ShaderInfos[0].Module = FindShader(ShaderLib, "Geo3D.vert");

    n.ShaderInfos[1].Stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    n.ShaderInfos[1].Module = FindShader(ShaderLib, "Color.frag");

    n.PushConstantHandle.pData = pPushConstantData;
    n.PushConstantHandle.Size = PushConstantSize;
    n.PushConstantHandle.Stage = PushConstantStage;

    n.DescriptorListHandle.ffType = ffTypes::FF_TYPE_CURVE_3D;

    return n;
}

static PipelineCreateInfos NewMesh2DPipeline(ShaderLibrary& ShaderLib, void *pPushConstantData, size_t PushConstantSize, VkShaderStageFlags PushConstantStage)
{
    PipelineCreateInfos n;

    n.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    n.PolygonMode = VK_POLYGON_MODE_LINE;
    n.LineWidth = 2;

    n.VertexSize = sizeof(float) * 7;
    n.VertexFormat.resize(2);
    n.VertexFormat[0].Format = VK_FORMAT_R32G32B32_SFLOAT;
    n.VertexFormat[0].Offset = 0;

    n.VertexFormat[1].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
    n.VertexFormat[1].Offset = sizeof(float) * 3;

    n.ShaderInfos.resize(2);
    n.ShaderInfos[0].Stage = VK_SHADER_STAGE_VERTEX_BIT;
    n.ShaderInfos[0].Module = FindShader(ShaderLib, "Geo3D.vert");

    n.ShaderInfos[1].Stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    n.ShaderInfos[1].Module = FindShader(ShaderLib, "Color.frag");

    n.PushConstantHandle.pData = pPushConstantData;
    n.PushConstantHandle.Size = PushConstantSize;
    n.PushConstantHandle.Stage = PushConstantStage;

    n.DescriptorListHandle.ffType = ffTypes::FF_TYPE_MESH_2D;

    return n;
}

static PipelineCreateInfos NewMesh3DPipeline(ShaderLibrary& ShaderLib, void *pPushConstantData, size_t PushConstantSize, VkShaderStageFlags PushConstantStage)
{
    PipelineCreateInfos n;

    n.Topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    n.PolygonMode = VK_POLYGON_MODE_LINE;
    n.LineWidth = 2;

    n.VertexSize = sizeof(float) * 7;
    n.VertexFormat.resize(2);
    n.VertexFormat[0].Format = VK_FORMAT_R32G32B32_SFLOAT;
    n.VertexFormat[0].Offset = 0;

    n.VertexFormat[1].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
    n.VertexFormat[1].Offset = sizeof(float) * 3;

    n.ShaderInfos.resize(2);
    n.ShaderInfos[0].Stage = VK_SHADER_STAGE_VERTEX_BIT;
    n.ShaderInfos[0].Module = FindShader(ShaderLib, "Geo3D.vert");

    n.ShaderInfos[1].Stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    n.ShaderInfos[1].Module = FindShader(ShaderLib, "Color.frag");

    n.PushConstantHandle.pData = pPushConstantData;
    n.PushConstantHandle.Size = PushConstantSize;
    n.PushConstantHandle.Stage = PushConstantStage;

    n.DescriptorListHandle.ffType = ffTypes::FF_TYPE_MESH_3D;

    return n;
}

static PipelineCreateInfos NewVectorField2DPipeline(ShaderLibrary& ShaderLib, void *pPushConstantData, size_t PushConstantSize, VkShaderStageFlags PushConstantStage)
{
    PipelineCreateInfos n;

    n.Topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    n.PolygonMode = VK_POLYGON_MODE_LINE;
    n.LineWidth = 2;

    n.VertexSize = sizeof(float) * 7;
    n.VertexFormat.resize(2);
    n.VertexFormat[0].Format = VK_FORMAT_R32G32B32_SFLOAT;
    n.VertexFormat[0].Offset = 0;

    n.VertexFormat[1].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
    n.VertexFormat[1].Offset = sizeof(float) * 3;

    n.ShaderInfos.resize(2);
    n.ShaderInfos[0].Stage = VK_SHADER_STAGE_VERTEX_BIT;
    n.ShaderInfos[0].Module = FindShader(ShaderLib, "Geo3D.vert");

    n.ShaderInfos[1].Stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    n.ShaderInfos[1].Module = FindShader(ShaderLib, "Color.frag");

    n.PushConstantHandle.pData = pPushConstantData;
    n.PushConstantHandle.Size = PushConstantSize;
    n.PushConstantHandle.Stage = PushConstantStage;

    n.DescriptorListHandle.ffType = ffTypes::FF_TYPE_VECTOR_FIELD_2D;

    return n;
}

static PipelineCreateInfos NewVectorField3DPipeline(ShaderLibrary& ShaderLib, void *pPushConstantData, size_t PushConstantSize, VkShaderStageFlags PushConstantStage)
{
    PipelineCreateInfos n;

    n.Topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    n.PolygonMode = VK_POLYGON_MODE_LINE;
    n.LineWidth = 2;

    n.VertexSize = sizeof(float) * 7;
    n.VertexFormat.resize(2);
    n.VertexFormat[0].Format = VK_FORMAT_R32G32B32_SFLOAT;
    n.VertexFormat[0].Offset = 0;

    n.VertexFormat[1].Format = VK_FORMAT_R32G32B32A32_SFLOAT;
    n.VertexFormat[1].Offset = sizeof(float) * 3;

    n.ShaderInfos.resize(2);
    n.ShaderInfos[0].Stage = VK_SHADER_STAGE_VERTEX_BIT;
    n.ShaderInfos[0].Module = FindShader(ShaderLib, "Geo3D.vert");

    n.ShaderInfos[1].Stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    n.ShaderInfos[1].Module = FindShader(ShaderLib, "Color.frag");

    n.PushConstantHandle.pData = pPushConstantData;
    n.PushConstantHandle.Size = PushConstantSize;
    n.PushConstantHandle.Stage = PushConstantStage;

    n.DescriptorListHandle.ffType = ffTypes::FF_TYPE_VECTOR_FIELD_3D;

    return n;
}

static PipelineCreateInfos UnknowPipeline()
{
    PipelineCreateInfos n;

    n.Topology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
    n.PolygonMode = VK_POLYGON_MODE_MAX_ENUM;
    n.LineWidth = 0;

    n.VertexSize = 0;

    n.DescriptorListHandle.ffType = ffTypes::FF_TYPE_UNKOWN;

    return n;
}

PipelineCreateInfos GetPipelineCreateInfos(ffTypes type, ShaderLibrary& ShaderLib, void *pPushConstantData, size_t PushConstantSize, VkShaderStageFlags PushConstantStage)
{
    typedef PipelineCreateInfos (*PipelineCreateInfos_func)(ShaderLibrary& ShaderLib, void *pPushConstantData, size_t PushConstantSize, VkShaderStageFlags PushConstantStage);
    struct {
        ffTypes type;
        PipelineCreateInfos_func f;
    } search_array[ffTypes::FF_TYPE_RANGE_SIZE] = {
        {ffTypes::FF_TYPE_CURVE_2D, NewCurve2DPipeline},
        {ffTypes::FF_TYPE_CURVE_3D, NewCurve3DPipeline},
        {ffTypes::FF_TYPE_MESH_2D, NewMesh2DPipeline},
        {ffTypes::FF_TYPE_MESH_3D, NewMesh3DPipeline},
        {ffTypes::FF_TYPE_VECTOR_FIELD_2D, NewVectorField2DPipeline},
        {ffTypes::FF_TYPE_VECTOR_FIELD_3D, NewVectorField3DPipeline},
    };

    for (unsigned int i = 0; i < ffTypes::FF_TYPE_RANGE_SIZE; ++i) {
        if (search_array[i].type == type)
            return search_array[i].f(ShaderLib, pPushConstantData, PushConstantSize, PushConstantStage);
    }
    return UnknowPipeline();
}

} // namespace ffGraph
} // namespace Vulkan