#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <cstdio>
#include <assert.h>

#if defined(__GNUC__) && (__GNUC__ >= 4)
    /**
     * @brief Force the compiler to throw a warning if the result of the marked function isn't used.
     * This macro is platform specific.
     */
    #define FORCE_USE_RESULT __attribute__((warn_unused_result))

    #define UNUSED_PARAM __attribute__((unused))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
    /**
     * @brief Force the compiler to throw a warning if the result of the marked function isn't used.
     * This macro is platform specific.
     */
    #define FORCE_USE_RESULT _Check_return_

    #define UNUSED_PARAM
#else
    /**
     * @brief Force the compiler to throw a warning if the result of the marked function isn't used.
     * This macro is platform specific.
     */
    #define FORCE_USE_RESULT

    #define UNUSED_PARAM
#endif

/**
 * @brief Report errors.
 */
struct Error {
    /**
     * Error value.
     */
    uint32_t m_value = NONE;

    static constexpr uint32_t NONE = 0;
    static constexpr uint32_t FUNCTION_FAILED = 1;
    static constexpr uint32_t FAILED_ALLOC = 2;
    static constexpr uint32_t FILE_NOT_FOUND = 4;
    static constexpr uint32_t FILE_ACCESS = 5;
    static constexpr uint32_t UNKNOWN = 6;

    /**
     * @brief Error's name as strings.
     */
    const char ErrorNames[6][19] =
    {
        "NONE", "FUNCTION_FAILED", "FAILED_ALLOC",
        "FILE_NOT_FOUND", "FILE_ACCESS_FAILED", "UNKNOWN"
    };

    /**
     * @brief Error constructor.
     *
     * @param uint32_t code[in] - Error code.
     */
    Error(const uint32_t code)
    : m_value(code)
    { }

    /**
     * @brief Error copy operator
     */
    Error& operator=(const Error& b) { m_value = b.m_value; return *this; }

    /**
     * @brief Error equal operator.
     *
     * @param const Error& b[in] - Error to compare.
     */
    bool operator==(const Error& b) const { return m_value == b.m_value; }

    /**
     * @brief Error equal operator.
     *
     * @param const uint32_t code[in] - Error code to compare.
     */
    bool operator==(const uint32_t code) const { return m_value == code; }

    /**
     * @brief Error different operator.
     *
     * @param const Error& b[in] - Error to compare.
     */
    bool operator!=(const Error& b) const { return m_value != b.m_value; }

    /**
     * @brief Error different operator.
     *
     * @param const uint32_t code[in] - Error code to compare.
     */
    bool operator!=(const uint32_t code) const { return m_value != code; }

    operator bool() const { return m_value != NONE; }
};

static constexpr int NUM_DEMO_VERTICES = 36;
static const float vertices[] = {
    -1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    -1.0f,-1.0f, 1.0f, 0.5, 0.5, 0.5,
    -1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    1.0f, 1.0f,-1.0f, 0.5, 0.5, 0.5,
    -1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    -1.0f, 1.0f,-1.0f, 0.5, 0.5, 0.5,
    1.0f,-1.0f, 1.0f, 0.5, 0.5, 0.5,
    -1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    1.0f, 1.0f,-1.0f, 0.5, 0.5, 0.5,
    1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    -1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    -1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    -1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    -1.0f, 1.0f,-1.0f, 0.5, 0.5, 0.5,
    1.0f,-1.0f, 1.0f, 0.5, 0.5, 0.5,
    -1.0f,-1.0f, 1.0f, 0.5, 0.5, 0.5,
    -1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    -1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    -1.0f,-1.0f, 1.0f, 0.5, 0.5, 0.5,
    1.0f,-1.0f, 1.0f, 0.5, 0.5, 0.5,
    1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    1.0f, 1.0f,-1.0f, 0.5, 0.5, 0.5,
    1.0f,-1.0f,-1.0f, 0.5, 0.5, 0.5,
    1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    1.0f,-1.0f, 1.0f, 0.5, 0.5, 0.5,
    1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    1.0f, 1.0f,-1.0f, 0.5, 0.5, 0.5,
    -1.0f, 1.0f,-1.0f, 0.5, 0.5, 0.5,
    1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    -1.0f, 1.0f,-1.0f, 0.5, 0.5, 0.5,
    -1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    -1.0f, 1.0f, 1.0f, 0.5, 0.5, 0.5,
    1.0f,-1.0f, 1.0f, 0.5, 0.5, 0.5
};


#endif // UTILS_H