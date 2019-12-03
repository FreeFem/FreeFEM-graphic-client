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

static void newGraphFrame(Root& r)
{
    static glm::vec3 Rotation;
    static float ZoomLevel;
    ImGui::NewFrame( );

    //ImGui::ShowDemoWindow();

    ImGui::Begin("Plot list");

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
            AddToGraph(RenderGraph, g, Shaders);
        }
        newGraphFrame(RenderGraph);
        UpdateUiPipeline(Ui);
        render( );
    }
}

}
} // namespace ffGraph