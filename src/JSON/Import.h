#ifndef IMPORT_H_
#define IMPORT_H_

#include <nlohmann/json.hpp>
#include "LabelTable.h"
#include "ThreadQueue.h"
#include "Array.h"

namespace ffGraph {
namespace JSON {

using json = nlohmann::json;

typedef Array (*FP_Import)(nlohmann::json Geometry, LabelTable& Colors);

Array ImportTetrahedronMesh3D(json Geometry, LabelTable& Colors);
Array ImportLineMesh3D(json Geometry, LabelTable& Colors);
Array ImportTriangleMesh2D(json Geometry, LabelTable& Colors);
Array ImportTriangleMesh3D(json Geometry, LabelTable& Colors);
Array ImportLineMesh2D(json Geometry, LabelTable& Colors);
Array ImportLineMesh3D(json Geometry, LabelTable& Colors);

}    // namespace JSON
}    // namespace ffGraph

#endif    // IMPORT_H_