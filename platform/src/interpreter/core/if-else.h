#ifndef IFELSE_H
#define IFELSE_H

#include "core.h"

class IfElseBlock{

public:
    struct CalculationMetaData {
        std::string type;
        std::size_t index;
    };

    IfElseBlock(const std::string& line, Core * core) : interpreter(core)
    {
        // check condition
        auto& pcb = interpreter->pcb;
        bool badOperator = true;
        const std::vector<std::string> operators = {"==", "!=", ">=", "<=", ">", "<"};
        for(auto& op : operators){
            const size_t index = line.find(op);

            if(index != std::string::npos){
                badOperator = false;
                std::string fop = interpreter->getWordAndItsEnd(line.substr(0, index)).first;
                std::string sop = interpreter->getWordAndItsEnd(line.substr(index + op.length())).first;

                if(interpreter->isVariable(fop)){
                    interpreter->variableAndTypeNameValidator(fop);
                    CalculationMetaData fop_m = loadValueToItsStack(fop);
                    fop.clear();

                    if(interpreter->isVariable(sop)){ //both are variable
                        interpreter->variableAndTypeNameValidator(sop);
                        CalculationMetaData sop_m = loadValueToItsStack(sop);
                        sop.clear();
                        interpreter->checkDataTypeOnOperation(2, fop_m.type.c_str(), sop_m.type.c_str());
                    } else { // first is variable and second is lvalue
                        CalculationMetaData sop_m = loadValueToItsStack(fop_m.type, sop);
                        sop.clear();
                    }
                    pcb->setIfElseMetaData(booleanOperation(fop_m.type, op));
                } else {
                    if(interpreter->isVariable(sop)){ // first is lvalue and second is variable
                        interpreter->variableAndTypeNameValidator(sop);
                        CalculationMetaData sop_m = loadValueToItsStack(sop);
                        sop.clear();
                        CalculationMetaData fop_m = loadValueToItsStack(sop_m.type, fop);
                        sop.clear();
                        pcb->setIfElseMetaData(booleanOperation(fop_m.type, op));
                    } else { // both are lvalue
                        std::string type = interpreter->isLvalueFloat(fop) ? "F64" : "int";
                        CalculationMetaData fop_m = loadValueToItsStack(type, fop);
                        fop.clear();
                        CalculationMetaData sop_m = loadValueToItsStack(type, sop);
                        sop.clear();
                        pcb->setIfElseMetaData(booleanOperation(type, op));
                    }
                }

                break;
            }
        }

        if(badOperator) LLCore::terminateExecution(E("Bad Operator"));
    }

private:

    CalculationMetaData loadValueToItsStack(const std::string &name)
    {
        const auto& value = interpreter->_values[interpreter->checkAccessToVariableAndScopizeName(name)];
        return loadValueToItsStack(value.first, value.second);
    }

    CalculationMetaData loadValueToItsStack(const std::string &type, const std::string &value)
    {
        return loadValueToItsStack(type, interpreter->stringToTypedAny(type, value));
    }

    CalculationMetaData loadValueToItsStack(const std::string &type, const std::any &value)
    {
        auto& pcb = interpreter->pcb;
        auto e = E("Invalid Data Type :: variable type : " + type);

        switch (interpreter->stringTypeToEnumType(type)) {
        case LLCore::DTS::Int:
            intStack.push(std::any_cast<int>(value));
            return {type, intStack.size() - 1};

        case LLCore::DTS::LInt:
            lintStack.push(std::any_cast<long>(value));
            return {type, lintStack.size() - 1};

        case LLCore::DTS::LLInt:
            llintStack.push(std::any_cast<long long>(value));
            return {type, llintStack.size() - 1};

        case LLCore::DTS::UInt:
            uintStack.push(std::any_cast<unsigned int>(value));
            return {type, uintStack.size() - 1};

        case LLCore::DTS::F32:
            f32Stack.push(std::any_cast<float>(value));
            return {type, f32Stack.size() - 1};

        case LLCore::DTS::F64:
            f64Stack.push(std::any_cast<double>(value));
            return {type, f64Stack.size() - 1};

        case LLCore::DTS::InValid: LLCore::terminateExecution(e); break;
        default: LLCore::terminateExecution(e);
        }

        LLCore::terminateExecution(e);
        return CalculationMetaData();
    }

    bool booleanOperation(const std::string& type, const std::string& op)
    {
        auto& pcb = interpreter->pcb;

        auto helper = [this, &op, &pcb] (auto oper1, auto oper2){
            auto e = E("Invalid Operator, used operator: " + op);

            switch (interpreter->operatorStringToOperatorEnum(op)) {
            case LLCore::BOPS::EQ: return oper1 == oper2;
            case LLCore::BOPS::NEQ: return oper1 != oper2;
            case LLCore::BOPS::GTEQ: return oper1 >= oper2;
            case LLCore::BOPS::LTEQ: return oper1 <= oper2;
            case LLCore::BOPS::GT: return oper1 > oper2;
            case LLCore::BOPS::LT: return oper1 < oper2;
            default: LLCore::terminateExecution(e);
            }

            LLCore::terminateExecution(e);
            return bool();
        };


        auto e = E("Invalid Data Type :: variable type : " + type);
        switch (interpreter->stringTypeToEnumType(type)) {
        case LLCore::DTS::Int:{
            int oper1 = intStack.top(); intStack.pop();
            int oper2 = intStack.top(); intStack.pop();
            return helper(oper2, oper1);
        }

        case LLCore::DTS::LInt:{
            long oper1 = lintStack.top(); lintStack.pop();
            long oper2 = lintStack.top(); lintStack.pop();
            return helper(oper2, oper1);
        }

        case LLCore::DTS::LLInt:{
            long long oper1 = llintStack.top(); llintStack.pop();
            long long oper2 = llintStack.top(); llintStack.pop();
            return helper(oper2, oper1);
        }

        case LLCore::DTS::UInt:{
            unsigned int oper1 = uintStack.top(); uintStack.pop();
            unsigned int oper2 = uintStack.top(); uintStack.pop();
            return helper(oper2, oper1);
        }

        case LLCore::DTS::F32:{
            float oper1 = f32Stack.top(); f32Stack.pop();
            float oper2 = f32Stack.top(); f32Stack.pop();
            return helper(oper2, oper1);
        }

        case LLCore::DTS::F64:{
            double oper1 = f64Stack.top(); f64Stack.pop();
            double oper2 = f64Stack.top(); f64Stack.pop();
            return helper(oper2, oper1);
        }

        case LLCore::DTS::InValid: LLCore::terminateExecution(e); break;
        default: LLCore::terminateExecution(e);
        }

        LLCore::terminateExecution(e);
        return bool();
    }

private:
    std::stack<int> intStack;
    std::stack<long> lintStack;
    std::stack<long long> llintStack;
    std::stack<unsigned int> uintStack;
    std::stack<float> f32Stack;
    std::stack<double> f64Stack;
    Core * interpreter;
};

#endif // IFELSE_H
