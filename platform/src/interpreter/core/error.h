#ifndef ERROR_H
#define ERROR_H

#include "../../core/base.h"

// #include <string>
// #include <vector>
// #include <tuple>
// #include <map>
// #include <any>
// #include <stack>
// #include <stdexcept>
// #include <fstream>
// #include <sstream>
// #include <limits>
// #include <stdarg.h>
// #include <algorithm>
// #include <math.h>
// #include <iostream>

// #if defined(_MSC_VER)
// #define NO_RETURN
// #define MAYBE_UNUSED
// #define DEPRECATED
// #define DEPRECATED_R(REASON)
// #else
// #define NO_RETURN [[noreturn]]
// #define MAYBE_UNUSED [[maybe_unused]]
// #define DEPRECATED [[deprecated]]
// #define DEPRECATED_R(REASON) [[deprecated(#REASON)]]
// #endif

// #define FASTER 1
// #define DEBUG 1

// #define INDEPENDENT
// #define NEED_IMPROVEMENTS

// #define NON_VOID(TP) return TP();
// #define FN_ATTRIBUTES(...) __VA_ARGS__
// #define FN_RETURN_TYPE(TP) TP
// #define FN_NAME
// #define LABEL(LBL) \
//     LBL:

// #if defined(ARDUINO) && DEBUG == 1
// #define DEBUG_PRINT(STRING) Serial.println(STRING)
// #else
// #if DEBUG == 0
// #define DEBUG_PRINT(STRING) std::cout << STRING << std::endl;
// #else
// #define DEBUG_PRINT(STRING)
// #endif
// #endif

// #if defined(ARDUINO)
// #define Print(VALUE) Serial.print(VALUE)
// #else
// #define Print(VALUE) std::cout << VALUE;
// #endif

// #if defined(ARDUINO)
// #define Println(VALUE) Serial.println(VALUE)
// #else
// #define Println(VALUE) std::cout << VALUE << std::endl
// #endif

// #if defined(ARDUINO)
// #define PrintCommand(VALUE) Serial.println(">> " + VALUE)
// #else
// #define PrintCommand(VALUE) std::cout << ">> " << VALUE << std::endl
// #endif

// #if FASTER == 1
// #define INLINE inline
// #else
// #define INLINE
// #endif

// #define PRETTY_ERROR(S, W) std::string("======================================================\n" \
//                                        "ERROR:\n"                                                 \
//                                        "\tdescription: " +                                        \
//                                        std::string(S) + "\n"                                      \
//                                                         "\tstack: \n"                             \
//                                                         "\t.\t internal: " +                      \
//                                        #W)

// #define ADD_STACK(E, L, LS, CPU)            \
//     std::string(E + "\n"                    \
//                     "\t.\t cpu: " CPU "\n"  \
//                     "\t.\t line number: " + \
//                 L + "\n"                    \
//                     "\t.\t line: " +        \
//                 LS + "\n"                   \
//                      "======================================================\n")

// #define EXF                                                    \
//     "\n\n"                                                     \
//     "======================================================\n" \
//     "Excecution Failed."                                       \
//     "\n======================================================" \
//     "\n\n"

// struct Error
// {
//     Error(const std::string &what, const std::string &lineNumber, const std::string &line)
//     {
//         this->line = line;
//         this->what = what;
//         this->lineNumber = lineNumber;
//     }

//     std::string cause() const
//     {
//         return ADD_STACK(what, lineNumber, line, "CPU1");
//     }

// private:
//     std::string what;
//     std::string line;
//     std::string lineNumber;
// };

// #define E(S, W) Error(PRETTY_ERROR(S, W), pcb->getLineNumber_s(), pcb->getCurrentLine())
// #define EPCB(S, W) Error(PRETTY_ERROR(S, W), getLineNumber_s(), getCurrentLine())

#endif // ERROR_H
