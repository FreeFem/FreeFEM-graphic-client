#ifndef LINEAR_ALLOC_H_
#define LINEAR_ALLOC_H_

#include <assert.h>
#include <cstdlib>

#ifdef _DEBUG

#include "Logger.h"

#endif

#include <algorithm>
#include <mutex>

namespace ffGraph {
namespace MemoryManagement {

class LinearAllocator {
    public:
        LinearAllocator(const size_t Size) { MemoryStart = malloc(Size); assert(MemoryStart != 0); TotalSize = Size; }

        ~LinearAllocator() {

            free(MemoryStart);
#ifdef _DEBUG
            LogInfo("LinearAllocator", "Total memory allocated %lu, total amount of memory used %lu.", TotalSize, Peak);
#endif
            TotalSize = 0;
        }

        void *Allocate(const size_t size) {
            Lock.lock();
            size_t padding = 0;
            size_t paddedAddress = 0;

            const size_t currentAddress = (std::size_t)MemoryStart + Offset;

            if (Offset % Alignment != 0) {
                padding = ComputePadding(currentAddress, Alignment);
            }
            if (Offset + padding + size > TotalSize)
                return nullptr;
            Offset += padding;
            size_t nAdress = currentAddress + padding;
            Offset += size;

#ifdef _DEBUG
            LogInfo("LinearAllocator", "Performing an allocation of %lu bytes, Offset is %lu. Adress : %x\n", size, Offset, nAdress);
#endif

            Used = Offset;
            Peak = std::max(Peak, Used);

            Lock.unlock();
            return (void *)nAdress;
        }

    private:
        void *MemoryStart = NULL;
        size_t Offset = 0;
        size_t TotalSize = 0;
        size_t Used = 0;
        size_t Peak = 0;
        std::mutex Lock;
        const size_t Alignment = sizeof(float);

        size_t ComputePadding(const size_t BaseAddress, const size_t Alignment) {
            const size_t mult = (BaseAddress / Alignment) + 1;
            const size_t AlignedAddress = mult * Alignment;
            const size_t Padding = AlignedAddress - BaseAddress;
            return Padding;
        }
};

extern LinearAllocator *GAlloc;

} // namespace ffGraph
} // namespace MemoryManagement

#endif // #define LINEAR_ALLOC_H_