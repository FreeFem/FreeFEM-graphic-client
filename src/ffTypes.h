#ifndef FF_TYPE_H_
#define FF_TYPE_H_

namespace ffGraph {

/**
 * @brief Containts value to represent FreeFem geometry type (used for pipeline creation).
 */
enum ffTypes {
    FF_TYPE_UNKOWN,
    FF_TYPE_CURVE_2D,
    FF_TYPE_CURVE_3D,
    FF_TYPE_MESH_2D,
    FF_TYPE_MESH_3D,
    FF_TYPE_VECTOR_FIELD_2D,
    FF_TYPE_VECTOR_FIELD_3D,
    FF_TYPE_BEGIN_RANGE = FF_TYPE_CURVE_2D,
    FF_TYPE_END_RANGE = FF_TYPE_VECTOR_FIELD_3D,
    FF_TYPE_RANGE_SIZE = (FF_TYPE_END_RANGE - FF_TYPE_BEGIN_RANGE) + 1
};

/**
 * @brief Function used to retrieve the enum value of a FreeFem geometry type from a string.
 *
 * @param type - C style srting containing the name of the type
 *
 * @return Returns ffTypes::FF_TYPE_UNKNOW if it cannot match the string to any type, else returns a valid ffTypes value.
 *
 */
inline ffTypes GetTypeValue(const char *type) {
    struct {
        ffTypes type;
        char name[16];
    } search_array[ffTypes::FF_TYPE_RANGE_SIZE] = {
        {FF_TYPE_CURVE_2D, "Curve2D"},
        {FF_TYPE_CURVE_3D, "Curve3D"},
        {FF_TYPE_MESH_2D, "Mesh2D"},
        {FF_TYPE_MESH_3D, "Mesh3D"},
        {FF_TYPE_VECTOR_FIELD_2D, "Vector2D"},
        {FF_TYPE_VECTOR_FIELD_3D, "Vector3D"},
    };

    for (unsigned int i = 0; i < ffTypes::FF_TYPE_RANGE_SIZE; ++i) {
        if (strcmp(search_array[i].name, type) == 0)
            return search_array[i].type;
    }
    return ffTypes::FF_TYPE_UNKOWN;
}

}

#endif // FF_TYPE_H_