#ifndef FUNCTION_H
#define FUNCTION_H

#include "variable.h"
#include "pcb-manager.h"

struct FunctionType {
    LLCore::DTS type;
    size_t start = std::numeric_limits<size_t>::max();
    std::vector<std::pair<LLCore::DTS, std::string>> params;
    std::string invokeLine;
};

class Function {

public:
    Function(Core * core) : interpreter(core), manager(core)
    {}

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME declare(const std::string &line)
    {
        FunctionType *fn = new FunctionType;
        std::string name, param;
        bool nameEnded = false, paramsEnded = false, commaSeen = false;

        auto manageParam = [&]() {
            auto x = Variable::nameAndType(param, interpreter, false);
            fn->params.push_back({interpreter->stringTypeToEnumType(x.first), x.second});
            param.clear();
            commaSeen = false;
        };

        for(auto ch = line.begin(); ch < line.end(); ch++){
            if((!nameEnded || (nameEnded && paramsEnded)) && *ch == ' ') continue;

            if(*ch == '(') {
                nameEnded = true;
                continue;
            }

            if(*ch == ')') {
                paramsEnded = true;
                manageParam();
                commaSeen = false;
                continue;
            }

            if(!nameEnded) name.push_back(*ch);

            if(nameEnded && !paramsEnded && *ch == ','){
                manageParam();
                commaSeen = true;
            }

            if(nameEnded && !paramsEnded && !commaSeen) param.push_back(*ch);

            if(paramsEnded) param.push_back(*ch);

            commaSeen = false;
        }

        fn->type = interpreter->stringTypeToEnumType(param);
        fn->start = interpreter->pcb->getLineNumber();

        interpreter->_functions.insert({name, fn});
        interpreter->pcb->setWithinFunctionDeclaration(true);
    }

    void invokeFunction(const std::string &line)
    {
        std::vector<std::pair<LLCore::DTS, std::string>> params;
        std::string name, param, copy;
        bool nameEnded = false, paramsEnded = false, commaSeen = false;
        auto& pcb = interpreter->pcb;

        size_t index = line.find("=");
        if(index != std::string::npos)
            copy = line.substr(index + 1);
        else
            copy = line;

        auto manageParams = [&]() {
            /// call by valueeeeeeeee!
            interpreter->trim(param);
            auto nop = interpreter->checkAccessToVariableAndScopizeName(param);
            params.push_back({interpreter->stringTypeToEnumType(interpreter->_values[nop].first), nop});

            param.clear();
        };

        for(auto ch = copy.begin(); ch < copy.end(); ch++){
            if((!nameEnded || (nameEnded && paramsEnded)) && *ch == ' ') continue;

            if(*ch == '(') {
                nameEnded = true;
                continue;
            }

            if(*ch == ')') {
                paramsEnded = true;
                manageParams();
                commaSeen = false;
                continue;
            }

            if(!nameEnded) name.push_back(*ch);

            if(nameEnded && !paramsEnded && *ch == ',')
            {
                manageParams();
                commaSeen = true;
            }

            if(nameEnded && !paramsEnded && !commaSeen) param.push_back(*ch);

            if(paramsEnded) {
                param.clear();
                break;
            }

            commaSeen = false;
        }

        if(interpreter->_functions.find(name) == interpreter->_functions.end())
            LLCore::terminateExecution(E(name + " is undefined!", loadValueToItsStack));


        auto& fn = interpreter->_functions[name];

        /// check data type
        if(fn->params.size() != params.size())
            LLCore::terminateExecution(E("You Passed " +
                                     std::to_string(params.size()) + " arguments but need " +
                                     std::to_string(fn->params.size()), loadValueToItsStack));

        size_t sz = params.size();
        for(size_t i = 0; i < sz; i++){
            if(params[i].first != fn->params[i].first){
                LLCore::terminateExecution(E("You Passed Parameter " + params[i].second + " with " +
                                         interpreter->enumTypeToStringType(params[i].first)+ " type that is incompatible with  " +
                                         interpreter->enumTypeToStringType(fn->params[i].first), loadValueToItsStack));
            }

            interpreter->_refs.insert({interpreter->scopizeName(fn->params[i].second, {pcb->scope.first + 1, pcb->scope.second}),
                          params[i].second});
        }

        pcb->addReturnAddress(pcb->getLineNumber());
        manager.goTo(fn->start + 1, interpreter);
        pcb->addToCallStack(name);
        fn->invokeLine = line;
    }

    void returnState(const std::string &line)
    {
        auto& pcb = interpreter->pcb;
        auto& fn = interpreter->_functions[pcb->getLastInvokedFn()];
        pcb->setReturnValue(ExpressionSolver(line, interpreter).result());

        // scope mager
        manager.removeVariablesAllocatedInThisScope(interpreter);
//        pcb->scope.first--;
        --pcb->scope;

        // generate expression to solve
        std::string &ln = fn->invokeLine;
        size_t index = ln.find(pcb->getLastInvokedFn());
        auto e = E("Invalid Data Type", changeVariable);

        switch (fn->type) {
        case LLCore::DTS::Int:
            ln.replace(index, ln.size() - index, std::to_string(static_cast<int>(pcb->getReturnValue())));
            break;

        case LLCore::DTS::LInt:
            ln.replace(index, ln.size() - index, std::to_string(static_cast<long>(pcb->getReturnValue())));
            break;

        case LLCore::DTS::LLInt:
            ln.replace(index, ln.size() - index, std::to_string(static_cast<long long>(pcb->getReturnValue())));
            break;

        case LLCore::DTS::UInt:
            ln.replace(index, ln.size() - index, std::to_string(static_cast<unsigned int>(pcb->getReturnValue())));
            break;

        case LLCore::DTS::F32:
            ln.replace(index, ln.size() - index, std::to_string(static_cast<float>(pcb->getReturnValue())));
            break;

        case LLCore::DTS::F64:
            ln.replace(index, ln.size() - index, std::to_string(static_cast<double>(pcb->getReturnValue())));
            break;

        case LLCore::DTS::InValid: LLCore::terminateExecution(e); break;
        default: LLCore::terminateExecution(e);
        }


        Variable(interpreter).assignment(ln, 0, true);
//        runCommand(ln);

        // free values
        fn->invokeLine.clear();
        pcb->setWithinFunctionCall(false);
        pcb->popCallStack();

        // goto
        manager.goTo(pcb->retAdd() + 1, interpreter);
    }

private:
    Core * interpreter;
    PCBManager manager;
};

#endif // FUNCTION_H
