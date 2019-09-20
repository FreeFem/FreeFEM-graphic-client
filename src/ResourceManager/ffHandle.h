#ifndef FF_HANDLE_H_
#define FF_HANDLE_H_

#include <cstdint>

namespace ffGraph {

/**
 * @brief All type of handle.
 */
enum ffHandleType : uint16_t {
    FF_HANDLE_TYPE_EMPTY,
    FF_HANDLE_TYPE_SHADER,
    FF_HANDLE_TYPE_IMAGE,
    FF_HANDLE_TYPE_MESH,
    FF_HANDLE_TYPE_STACK,
    FF_HANDLE_TYPE_COUNT
};

/**
 * @brief Data structure used to keep track of items in the ffGraph::ffResourceManager.
 */
union ffHandle {
    uint64_t Handle;
    struct ffHandleData {
        uint16_t Type;
        uint16_t UniqueBytes;
        uint32_t Cell;
    } Data;

    inline bool operator==(const ffHandle& a) { return a.Handle == Handle; };

    inline ffHandle operator=(const ffHandle& a) { Handle = a.Handle; return *this; }
};

static constexpr ffHandle InvalidHandle = {(uint64_t)UINT16_MAX << 32 | (uint64_t)UINT32_MAX};

inline bool ffIsHandleValid(ffHandle Handle) {return Handle == InvalidHandle;}

inline ffHandle ffNewHandle(uint16_t Type, uint16_t UniqueBytes, uint32_t Index) { return {(uint64_t)Type << 48 | (uint64_t)UniqueBytes << 32 | Index}; }

} // namespace ffGraph

#endif // FF_HANDLE_H_