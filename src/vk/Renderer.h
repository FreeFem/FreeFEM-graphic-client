#ifndef RENDERER_H_
#define RENDERER_H_

#include "../util/utils.h"
#include "Buffers.h"
#include "Camera.h"
#include "GraphContext.h"
#include "GraphManager.h"
#include "Shader.h"

namespace FEM {

namespace gr {

class Renderer;

class Pipeline {
   public:
    ErrorValues init(const Manager&, const Context&, Renderer&);

    void destroy(const Manager&, const Context&);

    size_t VerticesCount( ) const {
        size_t ret = 0;
        for (auto const VBuffer : VertexBuffers) {
            ret += VBuffer.getVerticesCount( );
        }
        return ret;
    }

    void update(Renderer& grr)
    {
    }

    ErrorValues reload(const Manager&, const Context&, Renderer&);

    ErrorValues render(const Manager&, Context&, Renderer&);

    bool updated = false;

    VkPipeline PipelineHandle;
    VkPipelineLayout PipelineLayout;
    VkRenderPass RenderPass;
    std::vector<VkFramebuffer> Framebuffers;

    std::vector<VertexBuffer> VertexBuffers;

    VkDescriptorSetLayout DescriptorSetLayout;
    Shader ShaderModules;
    std::vector<VkDescriptorSet> DescriptorSets;
    std::vector<UniformBuffer> UniformBuffers;

    Camera::UniformCamera PushCamera;
    glm::mat4 PushMatrix = glm::mat4(1.0f);
};

class PipelineList : public NonCopyable {
    public:
        bool updated = false;

        operator  bool() { return updated; }
        Pipeline& operator[](size_t idx) { return Pipelines[idx]; }
        std::vector<Pipeline> Pipelines;
};

class Renderer : public NonCopyable {
   public:
    ErrorValues init(const Manager& grm, const Context& grc)
    {
        Dimension = RenderDimension::_Unknow;
        Pipeline basePipeline;
        basePipeline.ShaderModules.init(grm.Device, "shaders/vertex.spirv", "shaders/fragment.spirv");
        VertexBuffer buffer;
        buffer.init(grm, (void *)vertices, NUM_DEMO_VERTICES, SIZE_DEMO_VERTEX,
            {{0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 0}, {1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3}},
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        basePipeline.VertexBuffers.push_back(buffer);
        basePipeline.init(grm, grc, *this);
        List.Pipelines.push_back(basePipeline);
        return ErrorValues::NONE;
    }

    void destroy(const Manager& grm, const Context& grc);

    ErrorValues render(const Manager&, Context&);

    void update() { for (auto pipelines : List.Pipelines) { pipelines.update(*this); } };

    enum RenderDimension { _Unknow = -1, _2D, _3D };
    RenderDimension Dimension;

    Camera Cam;

    PipelineList List;
};

}    // namespace gr

}    // namespace FEM

#endif    // RENDERER_H_