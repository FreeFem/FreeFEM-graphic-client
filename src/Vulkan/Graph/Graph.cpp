#include "Root.h"
#include <iostream>

namespace ffGraph {
namespace Vulkan {

static int SearchForPlot(Root r, uint16_t PlotID)
{
    for (size_t i = 0; i < r.Plots.size(); ++i) {
        if (r.Plots[i].PlotID == PlotID)
            return i;
    }
    return -1;
}

static int SearchForMesh(Plot p, uint16_t MeshID)
{
    for (size_t i = 0; i < p.Meshes.size(); ++i) {
        if (p.Meshes[i].MeshID == MeshID)
            return i;
    }
    return -1;
}

static void CreateMesh(Plot& p, ConstructedGeometry& g)
{
    Mesh n;

    n.MeshID = g.MeshID;
    n.Geometries.push_back(g.Geo);
    GeoUiData tmp;
    n.UiInfos.push_back(tmp);
    n.Tranform = glm::mat4(1.0f);

    p.Meshes.push_back(n);
}

static void CreatePlot(Root& r, ConstructedGeometry& g)
{
    Plot n;

    n.PlotID = g.PlotID;
    n.Transform = glm::mat4(1.0f);

    CreateMesh(n, g);

    r.Plots.push_back(n);
}

static void AddToMesh(Mesh& m, ConstructedGeometry& g)
{
    m.Geometries.push_back(g.Geo);
    GeoUiData tmp;
    m.UiInfos.push_back(tmp);
}

static void AddToPlot(Plot& p, ConstructedGeometry& g)
{
    int MeshIndex = SearchForMesh(p, g.MeshID);

    if (MeshIndex == -1) {
        CreateMesh(p, g);
    } else {
        AddToMesh(p.Meshes[MeshIndex], g);
    }
}

void AddToGraph(Root& r, ConstructedGeometry& g)
{
    int PlotIndex = SearchForPlot(r, g.PlotID);

    if (PlotIndex == -1) {
        CreatePlot(r, g);
    } else {
        AddToPlot(r.Plots[PlotIndex], g);
    }
}

static void MeshTraversal(Mesh m)
{
    std::cout << "\t\t- Mesh " << m.MeshID << " containing " << m.Geometries.size() << " unique geometries.\n";
}

static void PlotTraversal(Plot p)
{
    std::cout << "\tPlot " << p.PlotID << " containing " << p.Meshes.size() << " object(s).\n";
    for (size_t i = 0; i < p.Meshes.size(); ++i) {
        MeshTraversal(p.Meshes[i]);
    }
}

void GraphTraversal(Root r)
{
    std::cout << "Traversing Graph :\n";
    for (size_t i = 0; i < r.Plots.size(); ++i) {
        PlotTraversal(r.Plots[i]);
    }
}

}
}