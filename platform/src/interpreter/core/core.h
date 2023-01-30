#ifndef CORE_H
#define CORE_H

#include "expression-solver.h"

/**
 * ----- llcore
 * ----- modules
 * ----- core
 * ----- PCB
 * ----- front
 */

class Core : public LLCore {
public:

    Core()
    {}

    Core(const std::string& filename) : LLCore(filename)
    {}

    void checkDataTypeOnOperation(int n, ...)
    {
        va_list pointer;
        va_start(pointer, n);

        std::string defType = va_arg(pointer, char*);
        auto e = E("Operatiorn of Incompatible Data types, all operands must have " +
                       defType + " data type in this expression.");

        for(int i = 1; i < n; i++){
            std::string tmp = va_arg(pointer, char*);
            if(tmp != defType) terminateExecution(e);
        }

        va_end(pointer);
    }

    std::any stringToTypedAny(const std::string &type, const std::string &value)
    {
        double v = ExpressionSolver(value, this).result();
        auto e = E("Invalid Data Type :: variable type : " + type);

        switch(stringTypeToEnumType(type)){
        case DTS::Int: return static_cast<int>(v);
        case DTS::LInt: return static_cast<long>(v);
        case DTS::LLInt: return static_cast<long long>(v);
        case DTS::UInt: return static_cast<unsigned int>(v);
        case DTS::F32: return static_cast<float>(v);
        case DTS::F64: return static_cast<double>(v);
        case DTS::InValid: terminateExecution(e); break;
        default: terminateExecution(e);
        }

        terminateExecution(e);
        return std::any();
    }
};

#endif // CORE_H
