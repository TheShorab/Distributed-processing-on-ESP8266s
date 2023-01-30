#ifndef VARIABLE_H
#define VARIABLE_H

#include "core.h"
#include "scope.h"

#include <string>
#include <vector>
#include <any>

class Variable {

public:

    Variable(Core * core) : interpreter(core)
    {}

    static std::pair<std::string, std::string>
    nameAndType(const std::string& line, LLCore * interpreter, bool scopize = true)
    {
        const auto& pcb = interpreter->pcb;
        std::vector<std::string> typeName;
        std::string word; size_t index;

        for(size_t i = 0; i < line.length(); i++){
            std::tie(word, index) = interpreter->getWordAndItsEnd(line, i);
            interpreter->variableAndTypeNameValidator(word);
            if(!word.empty()) typeName.push_back(word);
            i = index;
        }

        if(typeName.size() != 2)
            LLCore::terminateExecution(E(
                "invalid instruction, in a line of patrick code, you can create only one variable!"));

        if(scopize) return {typeName[0], interpreter->scopizeName(typeName[1], interpreter->pcb->scope)};

        return {typeName[0], typeName[1]};
    }

    void declare(const std::string& line)
    {
        size_t indexOfAssign = line.find('=');

        if(indexOfAssign != std::string::npos) {
            std::tie(this->type, this->name) = nameAndType(line.substr(0, indexOfAssign), interpreter);
            this->value = interpreter->
                          stringToTypedAny(this->type,
                                           interpreter->
                                           trim_copy(line.substr(indexOfAssign + 1)));
        } else {
            std::tie(this->type, this->name) = nameAndType(line, interpreter);
            this->value = interpreter->stringToTypedAny(this->type, "0");
        }

        this->scope = interpreter->pcb->scope;

        LLCore::TypedValue newValue = toValue();
        interpreter->_values.insert({getName(), newValue});
    }

    std::string checkAccessToVariableAndScopizeName(const std::string &name)
    {
        return interpreter->checkAccessToVariableAndScopizeName(name);
    }

    double accessVariable(const std::string &name)
    {
        return interpreter->accessVariable(name);
    }

    void assignment(const std::string& expression, size_t opIndex, bool find = false)
    {
        if(find) opIndex = expression.find("=");
        double executionResult = ExpressionSolver(expression.substr(opIndex + 1), interpreter).result();
        std::string name = expression.substr(0, opIndex);
        interpreter->trim(name);
        changeVariable(name, executionResult);
    }

    void changeVariable(const std::string &name, const std::string& value)
    {
        ///exception
        interpreter->checkValue(value);
        auto& v = interpreter->_values[checkAccessToVariableAndScopizeName(name)];
        auto& pcb = interpreter->pcb;
        auto e = E("Invalid Data Type :: variable name : " + name);

        switch(interpreter->stringTypeToEnumType(v.first)){
        case LLCore::DTS::Int:
            v.second = std::stoi(value);
            break;

        case LLCore::DTS::LInt:
            v.second = std::stol(value);
            break;

        case LLCore::DTS::LLInt:
            v.second = std::stoll(value);
            break;

        case LLCore::DTS::UInt:
            v.second = std::stoul(value);
            break;

        case LLCore::DTS::F32:
            v.second = std::stof(value);
            break;

        case LLCore::DTS::F64:
            v.second = std::stod(value);
            break;

        case LLCore::DTS::InValid: LLCore::terminateExecution(e); break;
        default: LLCore::terminateExecution(e);
        }
    }

    void changeVariable(const std::string &name, const double &value)
    {
        auto& v = interpreter->_values[checkAccessToVariableAndScopizeName(name)];
        auto& pcb = interpreter->pcb;
        auto e = E("Invalid Data Type :: variable name : " + name);

        switch(interpreter->stringTypeToEnumType(v.first)){
        case LLCore::DTS::Int:
            v.second = static_cast<int>(value);
            break;

        case LLCore::DTS::LInt:
            v.second = static_cast<long int>(value);
            break;

        case LLCore::DTS::LLInt:
            v.second = static_cast<long long>(value);
            break;

        case LLCore::DTS::UInt:
            v.second = static_cast<unsigned int>(value);
            break;

        case LLCore::DTS::F32:
            v.second = static_cast<float>(value);
            break;

        case LLCore::DTS::F64:
            v.second = static_cast<double>(value);
            break;

        case LLCore::DTS::InValid: LLCore::terminateExecution(e); break;
        default: LLCore::terminateExecution(e);
        }
    }

    void deleteVariable(const std::string &line)
    {
        auto name =  checkAccessToVariableAndScopizeName(line);
        interpreter->_values.erase(name);
    }

    LLCore::TypedValue toValue(){
        return {this->type, this->value};
    }

    std::string getName() const {
        return this->name;
    }

private:
    std::string type;
    std::string name;
    std::any value;
    Scope scope;
    Core * interpreter;
};

#endif // VARIABLE_H
