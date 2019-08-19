#ifndef COMMON_H
#define COMMON_H

#include <vulkan/vulkan.h>

#if _DEBUG
    #define LAYERS_ENABLED 1
#else
    #define LAYERS_ENABLED 0
#endif

#ifdef __GNUC__ && (__GNUC__ >= 4)
    #define USE_RESULT __attribute__((warn_unused_result))
#elif _MSC_VER && (_MSC_VER >= 1700)
    #define USE_RESULT _Check_return_
#else
    #define USE_RESULT
#endif

class ReturnValue {
    public:
        static constexpr uint32_t NONE = 0;
        static constexpr uint32_t FUNCTION_FAILED = 1;
        static constexpr uint32_t ALLOC = 2;
        static constexpr uint32_t FILE_NOT_FOUND = 4;
        static constexpr uint32_t FILE_ACCESS = 5;
        static constexpr uint32_t UNKNOWN = 6;

        ReturnValue(uint32_t code)
        : m_value(code) { }

        ReturnValue& operator=(const ReturnValue& b) { m_value = b.m_value; return *this; }

        bool operator==(const ReturnValue& b) const { return m_value == b.m_value; }

        bool operator==(const uint32_t code) const { return m_value == code; }

        bool operator!=(const ReturnValue& b) const { return m_value != b.m_value; }

        bool operator!=(const uint32_t code) const { return m_value != code; }

        operator bool() const { return m_value != NONE; }

        uint32_t m_value = NONE;

        #define CHECK(call) \
            do { \
                ReturnValue v = call; \
                if (v) \
                    return v; \
            } while (0)

        #define CHECK_VK_FNC(call) \
            do { \
                VkResult result = call; \
                if (result != VK_SUCCESS) { return ReturnValue::FUNCTION_FAILED; } \
            } while (0)

        #define CHECK_MEMORY_ALLOC(ptr) \
            do { \
                if (ptr == 0) \
                    return ReturnValue::ALLOC; \
            } while (0);
};


#endif // COMMON_H