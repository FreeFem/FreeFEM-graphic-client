#ifndef MESH_H_
#define MESH_H_

#include <cstdint>
#include "Array.h"

namespace ffGraph {

enum GeometryType {
    Curve = (1 << 0),
    Mesh = (1 << 1),
    IsoValue = (1 << 2)
};

enum FreeFemType {
    Mesh,
    Border,
    IsoValues,
};

enum Dimension {
    _2D = (1 << 3),
    _3D = (1 << 4)
};

enum Primitive {
    Point = 0,
    LineList = 1,
    LineStrip = 2,
    Triangle = 3,
};

struct Geometry {
    FreeFemType ffType;
    uint16_t GeoInfos;
    Primitive Topology;

    Array data;
};

}

#endif // MESH_H_