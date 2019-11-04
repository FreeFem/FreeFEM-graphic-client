#include <imgui.h>
#include "Instance.h"
#include "Import.h"
#include "Graph/Root.h"

namespace ffGraph {
namespace Vulkan {

static void ListGeometry(Root& r, Plot& p)
{
    for (size_t i = 0; i < p.Meshes.size(); ++i) {
        int t = (int)i;

        if (ImGui::TreeNode((void *)(intptr_t)t, "Mesh %u", r.Plots[i].PlotID)) {

            for (size_t j = 0; j < p.Meshes[i].Geometries.size(); ++j) {
                int t1 = (int)j;

                if (ImGui::TreeNode((void *)(intptr_t)t1, "Geometry %u", p.Meshes[i].Geometries[j].refID)) {
                    ImGui::Checkbox("Select Geometry", &p.Meshes[i].UiInfos[j].Selected);
                    ImGui::Checkbox("Display Geometry", &p.Meshes[i].UiInfos[j].Render);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
    }
}

static void newGraphFrame(Root& r, bool *IndentTree)
{
    ImGui::NewFrame( );

    ImGui::ShowDemoWindow();

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

static void newFrame(bool *render) {
    ImGui::NewFrame( );

    ImGui::Begin("Test");

    ImGui::Checkbox("More testing", render);

    ImGui::End();

    ImGui::Render( );
}

void Instance::run(std::shared_ptr<std::deque<std::string>> SharedQueue, JSON::ThreadSafeQueue& GeometryQueue) {
    Root GraphRoot;
    bool IndentTree = true;

    while (!ffWindowShouldClose(m_Window)) {
        UpdateImGuiButton( );
        if (!SharedQueue->empty( )) {
            JSON::AsyncImport(SharedQueue->at(0), GeometryQueue);
            SharedQueue->pop_front( );
        }
        if (!GeometryQueue.empty()) {
            ConstructedGeometry g = GeometryQueue.pop();
            AddToGraph(GraphRoot, g);
        }
        newGraphFrame(GraphRoot, &IndentTree);
        UpdateUiPipeline(Ui);
        render( );
    }
}

}
} // namespace ffGraph