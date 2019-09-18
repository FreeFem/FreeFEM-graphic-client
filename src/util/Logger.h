/**
 * @file Logger.h
 * @brief Allow the application to report bugs and other informations.
 */
#ifndef LOGGER_H_
#define LOGGER_H_

#include <cassert>
#include <cstdarg>
#include <cstring>
#include <iostream>
#include <string>

namespace ffGraph {
/**
 * @brief Internal function to the macro ffGraph::GetCurrentLogLocation().
 *
 * @param file [in] - __FILE__ macro, gives the current file location.
 * @param line [in] - __LINE__ macro, gives the current line in the file.
 *
 * @return std::string - Returns a string formatted this way "${__FILE__} at line ${__LINE__} => ".
 */
inline std::string GetCurrentLogLocation_internal(const char *file, int line) {
    std::string ret(file);
    ret.append(" at line ");
    ret.append(std::to_string(line));
    ret.append(" => ");
    return ret;
}

/**
 * @brief Calls ffGraph::GetCurrentLogLocation_internal(), gets the position of this macro call in a file.
 * Giving exactly where the log function was called.
 *
 * @return std::string - Returns a string formatted this way "${__FILE__} at line ${__LINE__} => ".
 */
#define GetCurrentLogLocation( ) GetCurrentLogLocation_internal(__FILE__, __LINE__)

/**
 * @brief Takes a variable as input and make it's a string out of it.
 *
 * @return const char[] - A string made of the name of a variable (eg: GetVariableAsString(SomeEnumName) gives
 * "SomeEnumName").
 */
#define GetVariableAsString(Var) #Var

/**
 * @brief Logs a Error. When compiled in Debug, this log call will assert.
 *
 * @param Location [in] - Location of the log call, can be custom or ffGraph::GetCurrentLogLocation().
 * @param Format [in] - Printf like formatting string.
 * @param ... [in] - Printf like va_list.
 *
 * @return void
 */
inline void LogError(std::string Location, const char *Format, ...) {
    va_list list;
    va_start(list, Format);
    printf("ERROR :: Location -> %s | Message : ", Location.data( ));
    vprintf(Format, list);
    printf("\n");
    va_end(list);
#ifdef _DEBUG
    assert(0);
#endif
}

/**
 * @brief Logs a Warning.
 *
 * @param Location [in] - Location of the log call, can be custom or ffGraph::GetCurrentLogLocation().
 * @param Format [in] - Printf like formatting string.
 * @param ... [in] - Printf like va_list.
 *
 * @return void
 */
inline void LogWarning(std::string Location, const char *Format, ...) {
    va_list list;
    va_start(list, Format);
    printf("WARNING :: Location -> %s | Message : ", Location.data( ));
    vprintf(Format, list);
    printf("\n");
    va_end(list);
}

/**
 * @brief Logs a Information.
 *
 * @param Location [in] - Location of the log call, can be custom or ffGraph::GetCurrentLogLocation().
 * @param Format [in] - Printf like formatting string.
 * @param ... [in] - Printf like va_list.
 *
 * @return void
 */
inline void LogInfo(std::string Location, const char *Format, ...) {
    va_list list;
    va_start(list, Format);
    printf("INFORMATION :: Location -> %s | Message : ", Location.data( ));
    vprintf(Format, list);
    printf("\n");
    va_end(list);
}

}    // namespace ffGraph

#endif    // LOGGER_H_