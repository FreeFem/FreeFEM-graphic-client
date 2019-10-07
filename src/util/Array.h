/**
 * @file Array.h
 * @brief Simple abstraction over a bare bone C array.
 */
#ifndef ARRAY_H_
#define ARRAY_H_

#include <cstdlib>
#include <cstring>

namespace ffGraph {

/**
 * @brief Simple array struct.
 */
struct Array {
    // @brief Number of elements in the Array
    size_t ElementCount;
    // @brief Size of one element.
    size_t ElementSize;
    // @brief Actual array.
    void *Data;
};

/**
 * @brief Look if the Array allocation was successful.
 *
 * @param Array [in] - The Array on which the test will be performed.
 * @return bool -   true  : Array is ready.
 *                  false : Array isn't ready.
 */
inline bool isArrayReady(Array a) { return (a.Data) ? true : false; }

/**
 * @brief Create a new Array.
 *
 * @param ElementCount [in] - Number of elements in the Array.
 * @param ElementSize [in] - Size of one element (eg. sizeof(int)).
 *
 * @return ffGraph::Array - Allocate a new ffGraph::Array (use ffGraph::isArrayReady to check return value).
 */
inline Array ffNewArray(size_t ElementCount, size_t ElementSize) {
    return {ElementCount, ElementSize, malloc(ElementSize * ElementCount)};
};

/**
 * @brief Release Array memory and reset it.
 *
 * @param a [in] - ffGraph::Array to destroy.
 */
inline void DestroyArray(Array a) {
    free(a.Data);
    memset(&a, 0, sizeof(Array));
}

/**
 * @brief Copy size bytes to a array memory from a src opaque pointer
 *
 * @param dst [out] - ffGraph::Array where the data is copied.
 * @param src [in] - Source memory.
 * @param size [in] - Number of bytes copied.
 */
inline void ffMemcpyArray(Array dst, const void *src, size_t size) { memcpy(dst.Data, src, size); }

/**
 * @brief Copy ffGraph::Array's size bytes to a array memory from a src opaque pointer
 *
 * @param dst [out] - ffGraph::Array where the data is copied.
 * @param src [in] - Source memory.
 */
inline void ffMemcpyArray(Array dst, const void *src) { memcpy(dst.Data, src, dst.ElementCount * dst.ElementSize); }

/**
 * @brief Copy size bytes to a array memory from a src ffGraph::Array
 *
 * @param dst [out] - ffGraph::Array where the data is copied.
 * @param src [in] - Source ffGraph::Array.
 * @param size [in] - Number of bytes copied.
 */
inline void ffMemcpyArray(Array dst, Array src, size_t size) { memcpy(dst.Data, src.Data, size); }

/**
 * @brief Copy ffGraph::Array's size bytes to a array memory from a src ffGraph::Array
 *
 * @param dst [out] - ffGraph::Array where the data is copied.
 * @param src [in] - Source ffGraph::Array.
 */
inline void ffMemcpyArray(Array dst, Array src) { memcpy(dst.Data, src.Data, dst.ElementCount * dst.ElementSize); }

/**
 * @brief Apply function "void (* FUNCTION)(void *, size_t)" to each array element.
 * Function throws a warning at compilation, "pointer of type ‘void *’ used in arithmetic".
 *
 * @param a [in] - ffGraph::Array which the function will run on.
 * @param func [in] - Function pointer
 */
inline void ffArrayForEach(Array a, void (*func)(void *array, size_t idx)) {
    for (size_t idx = 0; idx < a.ElementCount; ++idx) {
        func((char *)a.Data + (idx * a.ElementSize), idx);
    }
}

}    // namespace ffGraph

#endif    // ARRAY_H_