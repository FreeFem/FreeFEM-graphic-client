#ifndef IMPORT_H_
#define IMPORT_H_

#include <nlohmann/json.hpp>
#include <vector>
#include "LabelTable.h"
#include "ThreadQueue.h"
#include "Array.h"

namespace ffGraph {
namespace JSON {

using json = nlohmann::json;

//Geometry ConstructGeometry(std::vector<float> Vertices, std::vector<uint32_t> Indices, std::vector<int> Labels);
//void ImportGeometry(json GeoJSON, ThreadSafeQueue& Queue, uint16_t PlotID);
void AsyncImport(std::string CompressedJSON, ThreadSafeQueue& Queue);

Geometry ConstructIsoLines(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float> Values, std::vector<float>& RefTriangle, std::vector<float>& KSub, float min, float max);

}    // namespace JSON
}    // namespace ffGraph

#endif    // IMPORT_H_