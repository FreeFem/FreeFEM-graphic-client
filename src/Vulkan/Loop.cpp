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

static const char table[GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_COUNT][38] = {
    "GEO_PRIMITIVE_TOPOLOGY_POINT_LIST",
    "GEO_PRIMITIVE_TOPOLOGY_LINE_LIST",
    "GEO_PRIMITIVE_TOPOLOGY_LINE_STRIP",
    "GEO_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST"
};

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
        ImGui::Text("Primitive : %s.", table[g.Description.PrimitiveTopology]);
        ImGui::End();
    }
}

static void ListGeometry(Root& r, Plot& p, size_t index)
{
    for (size_t i = 0; i < p.Meshes.size(); ++i) {
        int t = (int)i + index;

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

static void newGraphFrame(Root& r)
{
    static glm::vec3 Rotation;
    static float ZoomLevel;
    ImGui::NewFrame( );

    //ImGui::ShowDemoWindow();

    ImGui::Begin("Plot list");

    for (size_t i = 0; i < r.Plots.size(); ++i) {
        std::string name("Plot ");
        name.append(std::to_string(i));
        if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_None)) {
            ListGeometry(r, r.Plots[i], i);
        }
    }
    if (ImGui::SliderFloat("X", &Rotation.x, 0.f, 360.f)) {
        r.Cam.SetRotation(Rotation);
    }
    if (ImGui::SliderFloat("Y", &Rotation.y, 0.f, 360.f)) {
        r.Cam.SetRotation(Rotation);
    }
    if (ImGui::SliderFloat("Z", &Rotation.z, 0.f, 360.f)) {
        r.Cam.SetRotation(Rotation);
    }
    if (ImGui::Button("Reset rotation")) {
        Rotation.x = Rotation.y = Rotation.z = 0.f;
        r.Cam.SetRotation(Rotation);
    }
    if (ImGui::Button("Zoom +")) {
        CameraScroolEvents(r.Cam, 1.f);
    }
    ImGui::SameLine();
    if (ImGui::Button("Zoom -")) {
        CameraScroolEvents(r.Cam, -1.f);
    }
    ImGui::Separator();
    if (ImGui::Button("X +"))
        r.Cam.Translate(glm::vec3(0.25f * std::min(r.Cam.ZoomLevel, 1.f), 0.f, 0.f));
    ImGui::SameLine();
    if (ImGui::Button("X -"))
        r.Cam.Translate(glm::vec3(-0.25f * std::min(r.Cam.ZoomLevel, 1.f), 0.f, 0.f));

    ImGui::Separator();
    if (ImGui::Button("Y +"))
        r.Cam.Translate(glm::vec3(0.f, 0.25f * std::min(r.Cam.ZoomLevel, 1.f), 0.f));
    ImGui::SameLine();
    if (ImGui::Button("Y -"))
        r.Cam.Translate(glm::vec3(0.f, -0.25f * std::min(r.Cam.ZoomLevel, 1.f), 0.f));

    ImGui::Separator();
    if (ImGui::Button("Z +"))
        r.Cam.Translate(glm::vec3(0.f, 0.f, 0.25f * std::min(r.Cam.ZoomLevel, 1.f)));
    ImGui::SameLine();
    if (ImGui::Button("Z -"))
        r.Cam.Translate(glm::vec3(0.f, 0.f, -0.25f * std::min(r.Cam.ZoomLevel, 1.f)));
    ImGui::End();

    ImGui::Render();
}

void Instance::run(std::shared_ptr<std::deque<std::string>> SharedQueue, JSON::ThreadSafeQueue& GeometryQueue) {
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
        newGraphFrame(RenderGraph);
        UpdateUiPipeline(Ui);
        render( );
        //RenderGraph.CamUniform.Model = glm::rotate(RenderGraph.CamUniform.Model, 0.0001f, glm::vec3(0, 1, 0));
    }
}

glm::vec2 IsoValue(glm::vec2 T[3], glm::vec2 BarycentricPoint)
{
    glm::mat2 mat(glm::vec2(T[1].x - T[0].x, T[1].y - T[0].y), glm::vec2(T[2].x - T[0].x, T[2].y - T[0].y));

    return mat * BarycentricPoint * T[0];
}

}
} // namespace ffGraph