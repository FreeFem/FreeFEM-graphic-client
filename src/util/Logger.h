#ifndef LOGGER_H_
#define LOGGER_H_

#include <cassert>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>

namespace ffGraph {

inline std::string GetCurrentLogLocation_internal(const char *file, int line) { std::string ret(file); ret.append(" at line "); ret.append(std::to_string(line)); return ret; }
#define GetCurrentLogLocation() GetCurrentLogLocation_internal(__FILE__, __LINE__)

inline void LogError(std::string Location, const char *Format, ...) {
    va_list list;
    va_start(list, Format);
    printf("ERROR :: Location -> %s | Message :\n", Location.data());
    vprintf(Format, list);
    printf("\n");
    va_end(list);
#ifdef _DEBUG
    assert(0);
#endif
}
inline void LogWarning(std::string Location, const char *Format, ...) {
    va_list list;
    va_start(list, Format);
    printf("WARNING :: Location -> %s | Message :\n", Location.data());
    vprintf(Format, list);
    printf("\n");
    va_end(list);
}

inline void LogInfo(std::string Location, const char *Format, ...) {
    va_list list;
    va_start(list, Format);
    printf("INFORMATION :: Location -> %s | Message :\n", Location.data());
    vprintf(Format, list);
    printf("\n");
    va_end(list);
}

} // namespace ffGraph

#endif    // LOGGER_H_