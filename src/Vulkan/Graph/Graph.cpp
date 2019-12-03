#include <iostream>
#include "GlobalEnvironment.h"
#include "Root.h"

namespace ffGraph {
namespace Vulkan {

void BuildRenderBuffer(Root& r)
{
    if (r.RenderBuffer.Handle != VK_NULL_HANDLE) {
        DestroyBuffer(GetAllocator( ), r.RenderBuffer);
    }
    VkDeviceSize BufferSize = 0;

    for (size_t i = 0; i < r.RenderedGeometries.size(); ++i) {
        BufferSize += r.Geometries[r.RenderedGeometries[i]].Geo.size();
    }


    BufferCreateInfo CreateInfo = {};

    CreateInfo.vkData.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    CreateInfo.vkData.Size = BufferSize;
    CreateInfo.vkData.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    CreateInfo.vmaData.Usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    CreateInfo.vmaData.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    r.RenderBuffer = CreateBuffer(GetAllocator( ), CreateInfo);

    VkDeviceSize offset = 0;
    for (size_t i = 0; i < r.RenderedGeometries.size(); ++i) {
        r.Geometries[r.RenderedGeometries[i]].Geo.BufferOffset = offset;
        memcpy(((char *)r.RenderBuffer.Infos.pMappedData) + offset,
                    r.Geometries[r.RenderedGeometries[i]].Geo.Data.Data,
                    r.Geometries[r.RenderedGeometries[i]].Geo.size());
        offset += r.Geometries[r.RenderedGeometries[i]].Geo.size();
    }
}

void AddToGraph(Root& r, ConstructedGeometry& g, ShaderLibrary& ShaderLib)
{
    r.Update = true;

    r.Geometries.push_back(g);
    Geometry *p = &r.Geometries[r.Geometries.size() - 1].Geo;
    r.RenderedGeometries.push_back(r.Geometries.size() - 1);
    void *PushConstantPTR = (void *)&r.CamUniform;
    size_t PushConstantSize = sizeof(CameraUniform);

    if (!r.Pipelines.empty()) {
        bool add = true;
        for (size_t i = 0; i < r.Pipelines.size(); ++i) {
            if (r.Pipelines[i].CreationData.DescriptorListHandle.ffType == p->Type) {
                p->Description.PipelineID = i;
                add = false;
            }
        }
        if (add) {
            auto tmp = GetPipelineCreateInfos(g.Geo.Type, ShaderLib, PushConstantPTR, PushConstantSize, VK_SHADER_STAGE_VERTEX_BIT);
            r.Pipelines.resize(r.Pipelines.size() + 1);
            ConstructPipeline(r.Pipelines[r.Pipelines.size() - 1], tmp);
            p->Description.PipelineID = r.Pipelines.size() - 1;
        }
    } else {
        auto tmp = GetPipelineCreateInfos(g.Geo.Type, ShaderLib, PushConstantPTR, PushConstantSize, VK_SHADER_STAGE_VERTEX_BIT);
        r.Pipelines.resize(1);
        ConstructPipeline(r.Pipelines[0], tmp);
        p->Description.PipelineID = 0;
    }
    vkDeviceWaitIdle(GetLogicalDevice());
    BuildRenderBuffer(r);
}

void DestroyGraph(Root& r)
{
    for (size_t i = 0; i < r.Pipelines.size(); ++i) {
        DestroyPipeline(r.Pipelines[i]);
    }
    DestroyBuffer(GetAllocator( ), r.RenderBuffer);
}

// static void MeshTraversal(Mesh m)
// {
//     std::cout << "\t\t- Mesh " << m.MeshID << " containing " << m.Geometries.size() << " unique geometries.\n";
// }

// static void PlotTraversal(Plot p)
// {
//     std::cout << "\tPlot " << p.PlotID << " containing " << p.Meshes.size() << " object(s).\n";
//     for (size_t i = 0; i < p.Meshes.size(); ++i) {
//         MeshTraversal(p.Meshes[i]);
//     }
// }

// void GraphTraversal(Root r)
// {
//     std::cout << "Traversing Graph :\n";
//     for (size_t i = 0; i < r.Plots.size(); ++i) {
//         PlotTraversal(r.Plots[i]);
//     }
// }

}
}