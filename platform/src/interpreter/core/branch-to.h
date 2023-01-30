#ifndef BRANCHTO_H
#define BRANCHTO_H

#include "pcb-manager.h"

class BranchTo
{

public:

    CONSTRUCTOR BranchTo(Core* interpreter) : _branch(new BranchType), _interpreter(interpreter)
    {}

    FN_ATTRIBUTES(INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME declare(const std::string& line)
    {
        auto& pcb = _interpreter->pcb;
        const auto as = line.find("as");
        if(as == std::string::npos) LLCore::terminateExecution(E("a branch needs a name"));

        auto cpu = _interpreter->trim_copy(line.substr(0, as));
        auto name = _interpreter->trim_copy(line.substr(as + 2));

        const auto pos = name.find("__");
        if(pos != std::string::npos) name = name.substr(0, pos) + "_" + std::to_string(static_cast<int>(_interpreter->accessVariable(name.substr(pos + 2))));

        _branch->start = pcb->getLineNumber();

        if(cpu == "auto") return dynamicDeclaration(name);
        if(_interpreter->isDeclaredVariable(cpu)) return staticDeclaration(_interpreter->accessVariable(cpu), name);
        if(_interpreter->isNumber(cpu)) return staticDeclaration(_interpreter->toDouble(cpu), name);

        const auto i = cpu.find("CPU");
        if(i != std::string::npos)  return staticDeclaration(_interpreter->toDouble(cpu.substr(i + 1)), name);

        return  LLCore::terminateExecution(E("Invalid CPU ID"));
    }

private:

    FN_ATTRIBUTES(INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME staticDeclaration(unsigned int cpu, const std::string& name)
    {
        _branch->destination = cpu;
        declarationCore(name);
    }

    FN_ATTRIBUTES(INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME dynamicDeclaration(MAYBE_UNUSED const std::string& name)
    {
        auto& pcb = _interpreter->pcb;
        return  LLCore::terminateExecution(E("This feature is not implemented yet! Please use antoher versions of branch_to."));
    }

    FN_ATTRIBUTES(INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME declarationCore(const std::string& name)
    {
        _interpreter->pcb->setWithinBranchBlock(true);
        _interpreter->_branches.insert({name, _branch});
        PCBManager(_interpreter).setBranch(name);
    }

    BranchType * _branch;
    Core * _interpreter;
};

#endif // BRANCHTO_H
