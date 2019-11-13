#include <iostream>
#include "Pipeline.h"
#include "Root.h"
#include "GlobalEnvironment.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static bool CompareDescriptor(const GeometryDescriptor& a, const GeometryDescriptor& b)
{
    return ((a.PrimitiveTopology == b.PrimitiveTopology) && (a.PolygonMode == b.PolygonMode));
}

std::vector<GeometryDescriptor> GetUniqueGeometryRenderingType(Root& r)
{
    std::vector<GeometryDescriptor> ret;

    for (size_t i = 0; i < r.Plots.size(); ++i) {
        for (size_t j = 0; j < r.Plots[i].Meshes.size(); ++j) {
            for (size_t k = 0; k < r.Plots[i].Meshes[j].Geometries.size(); ++k) {

                const GeometryDescriptor Descrip = r.Plots[i].Meshes[j].Geometries[k].Description;

                if (r.Plots[i].Meshes[j].UiInfos[k].Render)
                    r.RenderedGeometries.push_back(&r.Plots[i].Meshes[j].Geometries[k]);
                if (ret.empty()) {
                    ret.push_back(Descrip);
                } else {
                    bool add = true;
                    for (size_t l = 0; l < ret.size(); ++l) {
                        if (CompareDescriptor(Descrip, ret[l])) {
                            add = false;
                        }
                    }
                    if (add)
                        ret.push_back(Descrip);
                }
            }
        }
    }
    return ret;
}

static bool CompareGeoPipeline(const GeometryDescriptor& a, Pipeline& b)
{
    return ((a.PrimitiveTopology == b.Topology) && (a.PolygonMode == b.PolygonMode));
}

void BuildRenderBuffer(Root& r)
{
    if (r.RenderBuffer.Handle != VK_NULL_HANDLE) {
        DestroyBuffer(GetAllocator( ), r.RenderBuffer);
    }
    VkDeviceSize BufferSize = 0;

    for (size_t i = 0; i < r.RenderedGeometries.size(); ++i) {
        BufferSize += r.RenderedGeometries[i]->size();
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
        r.RenderedGeometries[i]->BufferOffset = offset;
        memcpy(r.RenderBuffer.Infos.pMappedData + offset, r.RenderedGeometries[i]->Data.Data, r.RenderedGeometries[i]->size());
        offset += r.RenderedGeometries[i]->size();
    }
}

void ConstructCurrentGraphPipelines(Root& r, VkShaderModule Shaders[2])
{
    vkDeviceWaitIdle(GetLogicalDevice( ));
    if (!r.Update || r.Geometries.empty())
        return;
    r.RenderedGeometries.clear();
    std::vector<GeometryDescriptor> UniqueDescriptor = GetUniqueGeometryRenderingType(r);

    if (!r.Pipelines.empty()) {
        for (auto& P : r.Pipelines)
            DestroyPipeline(P);
        r.Pipelines.clear();
    }
    r.Pipelines.resize(UniqueDescriptor.size());

    for (size_t i = 0; i < UniqueDescriptor.size(); ++i) {
        if (!CreatePipeline(UniqueDescriptor[i], r.Pipelines[i], Shaders)) {
            std::cout << "Failed to create pipeline.\n";
            return;
        }
        for (size_t j = 0; j < r.RenderedGeometries.size(); ++j) {
            if (CompareGeoPipeline(r.RenderedGeometries[j]->Description, r.Pipelines[r.Pipelines.size() - 1]))
                r.RenderedGeometries[j]->Description.PipelineID = i;
        }
    }
    BuildRenderBuffer(r);
    r.Update = false;
}

void DestroyGraph(Root& r)
{
    for (auto& P : r.Pipelines)
        DestroyPipeline(P);
    r.Pipelines.clear();
    DestroyBuffer(GetAllocator( ), r.RenderBuffer);
}


} // namespace Vulkan
} // namespace ffGraph
