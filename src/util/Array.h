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
 * @param size_t ElementCount [in] - Number of elements in the Array.
 * @param size_t ElementSize [in] - Size of one element (eg. sizeof(int)).
 *
 * @return Array - Allocate a new Array (use ffGraph::isArrayReady to check return value).
 */
inline Array ffNewArray(size_t ElementCount, size_t ElementSize) { return {ElementCount, ElementSize, malloc(ElementSize * ElementCount)}; };

/**
 * @brief Release Array memory and reset it.
 *
 * @param Array [in] - Array to destroy.
 */
inline void ffDestroyArray(Array a) { free(a.Data); memset(&a, 0, sizeof(Array)); }

} // namespace ffGraph

#endif // ARRAY_H_