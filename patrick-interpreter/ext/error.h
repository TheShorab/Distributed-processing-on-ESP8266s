#ifndef ERROR_H
#define ERROR_H

#include <string>

#define E(S, W) std::string("======================================================\n"\
 "ERROR:\n"\
 "\tdescription: " + std::string(S) + "\n" \
 "\tstack: \n"\
 "\t.\t internal: " + #W)

#define EE(E, L, LS, CPU)          \
std::string(E + "\n"               \
 "\t.\t cpu: " CPU "\n"            \
 "\t.\t line number: " + L + "\n"  \
 "\t.\t line: " + LS + "\n"        \
 "======================================================\n")

#define EXF                        \
     "\n\n" \
     "======================================================\n"\
     "Excecution Failed."\
     "\n======================================================"\
     "\n\n"

#endif // ERROR_H
