#ifndef BASE_H
#define BASE_H

#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <any>
#include <stack>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <limits>
#include <stdarg.h>
#include <algorithm>
#include <math.h>

#ifndef ARDUINO
#include <iostream>
#else
#include <ESP8266WiFi.h>
#endif

#define CONSTRUCTOR
#define DESTRUCTOR

// #define SD_MODE 2

// #if (SD_MODE == 0)
// #define _SD_ SD
// #define _FILE_ File
// #endif

// #if (SD_MODE == 1)
// sdfat::SdFat sd;
// #define _SD_ sd
// #define _FILE_ sdfat::File
// #endif

// #if (SD_MODE == 2)
// sdfat::SdFat32 sd;
// #define _SD_ sd
// #define _FILE_ sdfat::File32
// #endif

// #if (SD_MODE == 3)
// sdfat::SdExFat sd;
// #define _SD_ sd
// #define _FILE_ sdfat::File32
// #endif

#if defined(_MSC_VER)
#define NO_RETURN
#define MAYBE_UNUSED
#define DEPRECATED
#define DEPRECATED_R(REASON)
#else
#define NO_RETURN [[noreturn]]
#define MAYBE_UNUSED [[maybe_unused]]
#define DEPRECATED [[deprecated]]
#define DEPRECATED_R(REASON) [[deprecated(#REASON)]]
#endif

#define FASTER 1
#define DEBUG 1

#define INDEPENDENT
#define NEED_IMPROVEMENTS

#define NON_VOID(TP) return TP();
#define FN_ATTRIBUTES(...) __VA_ARGS__
#define FN_RETURN_TYPE(TP) TP
#define FN_NAME
#define LABEL(LBL) \
    LBL:

#if FASTER == 1
#define INLINE inline
#else
#define INLINE
#endif

#ifdef ARDUINO
#define YIELD yield();
#define STR(string_literal) F(string_literal)
#define Print(VALUE) Serial.print(Platform::to_printing_value(VALUE))
#define Println(VALUE) Serial.println(Platform::to_printing_value(VALUE))
#define PrintCommand(VALUE)              \
    Serial.print(">> (FROM PATRICK): "); \
    Serial.println(Platform::to_printing_value(VALUE))

#define PrintMessage(VALUE)               \
    Serial.print(">> (FROM PLATFORM): "); \
    Serial.println(Platform::to_printing_value(VALUE))

#define ALLOC_TO_MEM PROGMEM

#if DEBUG == 1
#define PRINT_FUNC_INFO         \
    Print(STR("DEBUG: at "));   \
    Print(__PRETTY_FUNCTION__); \
    Print(STR(" -> "));

#define PRINT_FUNC_INFO_LN \
    PRINT_FUNC_INFO        \
    Print(STR("\n"));

#define DEBUG_PRINT(STRING) \
    PRINT_FUNC_INFO         \
    Print(STRING)

#define DEBUG_PRINTLN(STRING) \
    PRINT_FUNC_INFO           \
    Println(STRING)
#endif

#define PRETTY_ERROR(S) String(STR("======================================================\n" \
                                   "ERROR:\n"                                                 \
                                   "\tdescription: ") +                                       \
                               String(S) + STR("\n"                                           \
                                               "\tstack: \n"                                  \
                                               "\t.\t internal: PLATFORM ") +                 \
                               String(__PRETTY_FUNCTION__))

#define ADD_STACK_PLATFORM(E, CPU)                                                \
    String(E + STR("\n"                                                           \
                   "\t.\t cpu: ") +                                               \
           CPU + STR("\n"                                                         \
                     "======================================================\n")) \
        .c_str()

#define ADD_STACK_INTERPRETER(E, L, LS, CPU)                                     \
    String(E + STR("\n"                                                          \
                   "\t.\t cpu: ") +                                              \
           CPU + STR("\n"                                                        \
                     "\t.\t line number: ") +                                    \
           L + STR("\n"                                                          \
                   "\t.\t line: ") +                                             \
           LS + STR("\n"                                                         \
                    "======================================================\n")) \
        .c_str()

#else
#define YIELD
#define STR(string_literal) string_literal
#define Print(VALUE) std::cout << Platform::to_printing_value(VALUE);
#define Println(VALUE) std::cout << Platform::to_printing_value(VALUE) << std::endl;
#define PrintCommand(VALUE) std::cout << ">> " << Platform::to_printing_value(VALUE) << std::endl;

#define ALLOC_TO_MEM

#if DEBUG == 1
#define PRINT_FUNC_INFO \
    std::cout << STR("DEBUG: at ") << __PRETTY_FUNCTION__ << STR(" -> ");

#define PRINT_FUNC_INFO_LN \
    PRINT_FUNC_INFO        \
    std::cout << std::endl;

#define DEBUG_PRINT(STRING) \
    PRINT_FUNC_INFO         \
    std::cout << STRING;

#define DEBUG_PRINTLN(STRING) \
    PRINT_FUNC_INFO           \
    std::cout << STRING << std::endl;

#define PRETTY_ERROR(S) std::string(STR("======================================================\n" \
                                        "ERROR:\n"                                                 \
                                        "\tdescription: ") +                                       \
                                    std::string(S) + STR("\n"                                      \
                                                         "\tstack: \n"                             \
                                                         "\t.\t internal: PLATFORM ") +            \
                                    __PRETTY_FUNCTION__)

#define ADD_STACK_PLATFORM(E, CPU)                                                     \
    std::string(E + STR("\n"                                                           \
                        "\t.\t cpu: ") +                                               \
                CPU + STR("\n"                                                         \
                          "======================================================\n")) \
        .c_str()

#define ADD_STACK_INTERPRETER(E, L, LS, CPU)                            \
    std::string(E + STR("\n"                                            \
                        "\t.\t cpu: ") CPU STR("\n"                     \
                                               "\t.\t line number: ") + \
                L + STR("\n"                                            \
                        "\t.\t line: ") +                               \
                LS + STR("\n"                                           \
                         "======================================================\n"))

#else
#define DEBUG_PRINT(STRING)
#endif
#endif

#define DECLARE_INIT_MEM(TP, VAR, VAL) TP VAR ALLOC_TO_MEM = VAL
#define DECLARE_INIT(TP, VAR, VAL) TP VAR = VAL
#define DECLARE(TP, VAR) TP VAR

#define USE_TCP 0
#define USE_UDP 1
#define USE_NOW 2
#define USE_HTTP 3

#define NORMAL_EXECUTION 0
#define FORCE_SINGLE_CORE 1

#define EXF                                                        \
    STR("\n\n"                                                     \
        "======================================================\n" \
        "Execution Failed."                                        \
        "\n======================================================" \
        "\n\n")

namespace Platform
{

#if defined(ARDUINO)
    String to_string(const std::string &s)
    {
        return String(s.c_str());
    }

    String to_string(const char *s)
    {
        return String(s);
    }

#define TO_STRING(STRING) String(STRING)

    const char *to_printing_value(const std::string &s)
    {
        return s.c_str();
    }

    const char *to_printing_value(const String &s)
    {
        return s.c_str();
    }

    const char *to_printing_value(const char *s)
    {
        return s;
    }

#else

    std::string to_string(const std::string &s)
    {
        return s;
    }

    std::string to_string(const char *s)
    {
        return std::string(s);
    }

    const char *to_printing_value(const std::string &s)
    {
        return s.c_str();
    }

    const char *to_printing_value(const char *s)
    {
        return s;
    }

#define TO_STRING(STRING) std::to_string(STRING)

#endif

    namespace Base
    {
        namespace Flags
        {
            DECLARE_INIT(int8_t, network, USE_TCP);
            DECLARE_INIT(int8_t, execution, NORMAL_EXECUTION);
        }

#ifdef ARDUINO
        namespace Data
        {
            struct IPort
            {
                IPAddress ip;
                uint16_t port;
            };

            File file;

            DECLARE_INIT(int8_t, ID, -1);
        }
#else
        namespace Data
        {
            struct IPort
            {
                std::string ip;
                uint16_t port;
            };

            std::ifstream file;

            DECLARE_INIT(int8_t, ID, -1);
        }
#endif

        namespace Interpreter
        {

#define E(S) Platform::Base::Interpreter::Error( \
    Platform::to_printing_value(PRETTY_ERROR(Platform::to_printing_value(S))), pcb->getLineNumber_s(), pcb->getCurrentLine())

#define EPCB(S) Platform::Base::Interpreter::Error( \
    Platform::to_printing_value(PRETTY_ERROR(Platform::to_printing_value(S))), getLineNumber_s(), getCurrentLine())

            struct Error
            {
                Error(const std::string &what, const std::string &lineNumber, const std::string &line)
                {
                    this->line = line;
                    this->what = what;
                    this->lineNumber = lineNumber;
                }

                std::string cause() const
                {
                    return ADD_STACK_INTERPRETER(to_string(what), to_string(lineNumber),
                                                 to_string(line), STR("CPU1"));
                }

            private:
                std::string what;
                std::string line;
                std::string lineNumber;
            };
        }

        void terminate_execution(const std::string &e)
        {
            Println(ADD_STACK_PLATFORM(Platform::to_string(e), TO_STRING(Base::Data::ID)));
            Println(EXF);
        }

        namespace Branch
        {
            bool isOnBranch = false;
            bool isResultReady = false;
            bool isReadyToExecute = false;
            double result;
            std::vector<std::string> branchCode;
        }
    }
}

#define WEB_SERVER_PORT Platform::Base::Data::ID == 0 ? 8080 : 80
#define WEB_CLIENT_PORT Platform::Base::Data::ID == 0 ? 80 : 8080
#define UDP_PORT 443
#define TCP_PORT 88
#define NOW_PORT

#endif
