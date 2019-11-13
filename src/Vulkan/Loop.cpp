#include <imgui.h>
#include <chrono>
#include <string>
#include <memory>
#include <iostream>
#include "Instance.h"
#include "Import.h"
#include "Graph/Root.h"

namespace ffGraph {
namespace Vulkan {

static void GeoParamPanel(Root& r, Plot& p, Mesh& m, Geometry& g, GeoUiData& UiData)
{
    if (UiData.ShowParameterWindow) {
        std::string name("Geometry ");
        name.append(std::to_string(g.refID));
        ImGui::Begin(name.c_str());

        bool tmp;
        if (ImGui::Checkbox("Fill geometry", &tmp)) {
            g.Description.PolygonMode = GeometryPolygonMode::GEO_POLYGON_MODE_FILL;
        } else {
            g.Description.PolygonMode = GeometryPolygonMode::GEO_POLYGON_MODE_LINE;
        }
        ImGui::End();
    }
}

static void ListGeometry(Root& r, Plot& p)
{
    for (size_t i = 0; i < p.Meshes.size(); ++i) {
        int t = (int)i;

        if (ImGui::TreeNode((void *)(intptr_t)t, "Mesh %u", p.Meshes[i].MeshID)) {

            for (size_t j = 0; j < p.Meshes[i].Geometries.size(); ++j) {
                int t1 = (int)j;

                if (ImGui::TreeNode((void *)(intptr_t)t1, "Geometry %u", p.Meshes[i].Geometries[j].refID)) {
                    ImGui::Checkbox("Select Geometry", &p.Meshes[i].UiInfos[j].Selected);
                    ImGui::Checkbox("Display Geometry", &p.Meshes[i].UiInfos[j].Render);
                    if (ImGui::Button("Show parameters")) {
                        p.Meshes[i].UiInfos[j].ShowParameterWindow = !p.Meshes[i].UiInfos[j].ShowParameterWindow;
                    }
                    GeoParamPanel(r, p, p.Meshes[i], p.Meshes[i].Geometries[j], p.Meshes[i].UiInfos[j]);
                    ImGui::TreePop();
                }
            }
            if (ImGui::Checkbox("Select all geometries", &p.Meshes[i].Selected)) {
                for (size_t j = 0; j < p.Meshes[i].Geometries.size(); ++j) {
                    p.Meshes[i].UiInfos[j].Selected = p.Meshes[i].Selected;
                }
            }
            ImGui::TreePop();
        }
    }
}

static void newGraphFrame(Root& r, bool *IndentTree)
{
    ImGui::NewFrame( );

    ImGui::Begin("Graph");

    ImGui::Checkbox("Toggle tree indentation", IndentTree);

    if (!IndentTree)
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.0f);
    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
    if (ImGui::TreeNode("Test Tree Root")) {
        for (size_t i = 0; i < r.Plots.size(); ++i) {
            int tmp = (int)i;
            if (ImGui::TreeNode((void *)(intptr_t)tmp, "Plot %u", r.Plots[i].PlotID)) {
                ListGeometry(r, r.Plots[i]);
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();

    ImGui::Render();
}

void Instance::run(std::shared_ptr<std::deque<std::string>> SharedQueue, JSON::ThreadSafeQueue& GeometryQueue) {
    bool IndentTree = true;
    uint16_t RuntimeInternID = 0;
    VkShaderModule Modules[2] = {
        FindShader(Shaders, "Geo3D.vert"),
        FindShader(Shaders, "Color.frag")
    };

    InitCameraController(RenderGraph.Cam, 1280.f / 768.f, 90.f, CameraType::_3D);
    RenderGraph.Cam.Translate(glm::vec3(0.5, -0.5, 0));
    RenderGraph.CamUniform.Model = glm::mat4(1.0f);
    while (!ffWindowShouldClose(m_Window)) {
        UpdateImGuiButton( );
        if (!SharedQueue->empty( )) {
            JSON::AsyncImport(SharedQueue->at(0), GeometryQueue);
            SharedQueue->pop_front( );
        }
        if (!GeometryQueue.empty()) {
            ConstructedGeometry g = GeometryQueue.pop();
            g.Geo.refID = RuntimeInternID;
            RuntimeInternID += 1;
            AddToGraph(RenderGraph, g);
            ConstructCurrentGraphPipelines(RenderGraph, Modules);
        }
        newGraphFrame(RenderGraph, &IndentTree);
        UpdateUiPipeline(Ui);
        render( );
        RenderGraph.CamUniform.Model = glm::rotate(RenderGraph.CamUniform.Model, 0.0001f, glm::vec3(0, 1, 0));
    }
}

}
} // namespace ffGraph