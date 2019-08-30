#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <iostream>
#include <cassert>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include "NonCopyable.h"

class Logger : public FEM::NonCopyable {
    public:
        Logger() {};

        void loge(std::string location, std::string infos)
        {
            std::string data;
            time_t ti = time(0);
            char hour[16] = {};
            memcpy((void *)hour, ctime(&ti) + 11, 8);
            std::string t(hour);
            data = "Log [" + t + "]:: ERROR > " + location + " : " + infos + "\n";
            std::cerr << data;
#ifdef _DEBUG
            assert(0);
#endif
        }

        void logw(std::string location, std::string infos)
        {
            std::string data;
            time_t ti = time(0);
            char hour[16] = {};
            memcpy((void *)hour, ctime(&ti) + 11, 8);
            std::string t(hour);
            data = "Log [" + t + "] :: WARNING > " + location + " : " + infos + "\n";
            std::cerr << data;
        }

        void logi(std::string location, std::string infos)
        {
            std::string data;
            time_t ti = time(0);
            char hour[16] = {};
            memcpy((void *)hour, ctime(&ti) + 11, 8);
            std::string t(hour);
            data = "Log [" + t + "] :: INFO > " + location + " : " + infos + "\n";
            std::cerr << data;
        }

        std::string getLocation() {
            std::string location(__FILE__);
            location.append(" at ligne ");
            location.append(std::to_string(__LINE__));
            return location;
        }
    };

    static Logger LoggerSingleton;

#define FILE_LOCATION() LoggerSingleton.getLocation()

#define LOGE(Location, Infos) \
    do { \
        LoggerSingleton.loge(Location, Infos); \
    } while (0)

#define LOGW(Location, Infos) \
    do { \
        LoggerSingleton.logw(Location, Infos); \
    } while (0)

#define LOGI(Location, Infos) \
    do { \
        LoggerSingleton.logi(Location, Infos); \
    } while (0)

#endif // LOGGER_H_