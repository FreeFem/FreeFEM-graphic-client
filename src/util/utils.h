#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <cstdio>
#include <assert.h>

#if defined(__GNUC__) && (__GNUC__ >= 4)
    /**
     * @brief Force the compiler to throw a warning if the result of the marked function isn't used.
     * This macro is different for GNU, MSC or other compilers.
     */
    #define FORCE_USE_RESULT __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
    #define FORCE_USE_RESULT _Check_return_
#else
    #define FORCE_USE_RESULT
#endif

struct Error {
    uint32_t m_value = NONE;

    static constexpr uint32_t NONE = 0;
    static constexpr uint32_t FUNCTION_FAILED = 1;
    static constexpr uint32_t FAILED_ALLOC = 2;
    static constexpr uint32_t FILE_NOT_FOUND = 4;
    static constexpr uint32_t FILE_ACCESS = 5;
    static constexpr uint32_t UNKNOWN = 6;

    const char ErrorNames[6][19] =
    {
        "NONE", "FUNCTION_FAILED", "FAILED_ALLOC",
        "FILE_NOT_FOUND", "FILE_ACCESS_FAILED", "UNKNOWN"
    };

    Error(const uint32_t code)
    : m_value(code)
    { }

    Error& operator=(const Error& b) { m_value = b.m_value; return *this; }

    bool operator==(const Error& b) const { return m_value == b.m_value; }

    bool operator==(const uint32_t code) const { return m_value == code; }

    bool operator!=(const Error& b) const { return m_value != b.m_value; }

    bool operator!=(const uint32_t code) const { return m_value != code; }

    operator bool() const { return m_value != NONE; }
};

/** TMP DATA FOR TEST PURPOSE **/

struct vertex {
    float x, y, z;
    float r, g, b;
};

static const char VERTEX_SHADER_FILENAME[] = "shaders/vertex.spirv";
static const char FRAGMENT_SHADER_FILENAME[] = "shaders/fragment.spirv";

static constexpr int VERTEX_INPUT_BINDING = 0;

static constexpr int NUM_DEMO_VERTICES = 3;
static const vertex vertices[NUM_DEMO_VERTICES] =
{
	//      position             color
	{  0.5f,  0.5f,  0.0f,  0.1f, 0.8f, 0.1f },
	{ -0.5f,  0.5f,  0.0f,  0.8f, 0.1f, 0.1f },
	{  0.0f, -0.5f,  0.0f,  0.1f, 0.1f, 0.8f },
};

/**                           **/
#endif // UTILS_H