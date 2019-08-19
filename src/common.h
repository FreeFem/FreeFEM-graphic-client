#ifndef COMMON_H
#define COMMON_H

#include <vulkan/vulkan.h>

#if defined(_DEBUG)
    #define LAYERS_ENABLED 1
#else
    #define LAYERS_ENABLED 0
#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
    #define USE_RESULT __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
    #define USE_RESULT _Check_return_
#else
    #define USE_RESULT
#endif

class ReturnError {
    public:
        uint32_t m_value = NONE;

        static constexpr uint32_t NONE = 0;
        static constexpr uint32_t FUNCTION_FAILED = 1;
        static constexpr uint32_t FAILED_ALLOC = 2;
        static constexpr uint32_t FILE_NOT_FOUND = 4;
        static constexpr uint32_t FILE_ACCESS = 5;
        static constexpr uint32_t UNKNOWN = 6;

    ReturnError(const uint32_t code)
    : m_value(code) { }

    ReturnError& operator=(const ReturnError& b) { m_value = b.m_value; return *this; }

    bool operator==(const ReturnError& b) const { return m_value == b.m_value; }

    bool operator==(const uint32_t code) const { return m_value == code; }

    bool operator!=(const ReturnError& b) const { return m_value != b.m_value; }

    bool operator!=(const uint32_t code) const { return m_value != code; }

    operator bool() const { return m_value != NONE; }
};

#define CHECK(call) \
    do { \
        ReturnError v = call; \
        if (v) \
            return v; \
    } while (0)

#define CHECK_VK_FNC(call) \
    do { \
        VkResult result = call; \
        if (result != VK_SUCCESS) { return ReturnError::FUNCTION_FAILED; } \
    } while (0)

#define CHECK_MEMORY_ALLOC(ptr) \
    do { \
        if (ptr == 0) \
            return ReturnError::FAILED_ALLOC; \
    } while (0)

struct GPUDeviceCapabilities {
    size_t m_uniformBufferBindOffsetAlignment = UINT32_MAX;
    size_t m_uniformBufferMaxRange = 0;
    size_t m_storageBufferBindOffsetAlignment = UINT32_MAX;
    size_t m_storageBufferMaxRange = 0;
    size_t m_textureBufferBindOffsetAlignment = UINT32_MAX;
    size_t m_textureBufferMaxRange = 0;
    uint32_t m_pushContantsSize = 128;

    uint32_t m_gpuVendor = 0;
    uint8_t m_minorApiVersion = 0;
    uint8_t m_majorApiVersion = 0;
};


#endif // COMMON_H