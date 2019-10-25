#include <nlohmann/json.hpp>
#include <fstream>
#include <Mesh.h>
#include "deserializer.h"
#include "Logger.h"

namespace ffGraph {
namespace JSON {

using json = nlohmann::json;

uint16_t ChooseGeoTypeAndDimension(std::string& GeoType)
{
    if (GeoType.compare("Curve2D") == 0)
        return GeometryType::Curve | Dimension::_2D;
    else if (GeoType.compare("Curve3D") == 0)
        return GeometryType::Curve | Dimension::_3D;
    else if (GeoType.compare("Mesh2D") == 0)
        return GeometryType::Mesh | Dimension::_2D;
    else if (GeoType.compare("Mesh3D") == 0)
        return GeometryType::Mesh | Dimension::_3D;
    else
        return UINT16_MAX;
}

}    // namespace JSON
}    // namespace ffGraph