#ifndef LOOP_H
#define LOOP_H

#include "core.h"
#include "pcb-manager.h"
#include "variable.h"

//#define LOOP(TP, TP_CONVERTER)\
//auto M = PCBManager(interpreter); \
//auto op = M.loop()->op;\
//    TP dest = TP_CONVERTER(M.loop()->dest);\
//    TP step = TP_CONVERTER(M.loop()->step);\
//    for(TP i = TP_CONVERTER(initValue); loopCondition(i,op,dest); i += step){\
//        for(const auto& instruction : instructions){\
//            runCommand(instruction);\
//    }\
//}

class Loop {

public:
    Loop(Core * interpreter) : interpreter(interpreter)
    {}

    void run(const std::string &line, bool runFaster)
    {
        // check this line has how may ,s
        const std::string::difference_type count = std::count(line.begin(), line.end(), ',');

        //exception
        if(count  != 1) return;

        //exception
        size_t opFIndx, opLength;
        std::tie(opFIndx, opLength) = findLoopOperator(line);
        if(opFIndx == std::string::npos) return;

        size_t stepIndex = line.find("step");
        if(stepIndex != std::string::npos && stepIndex < opFIndx + opLength) return;

        std::string secondPart = line.substr(opFIndx + opLength + 1);

        std::string initPart = line.substr(0, opFIndx);
        interpreter->trim(initPart);

        secondPart = interpreter->removeSpaces(secondPart);
        size_t assignIndex = secondPart.find("=");
        if(assignIndex == std::string::npos) return;

        Variable(interpreter).declare(initPart);
        auto tn = Variable::nameAndType(initPart.substr(0, initPart.find('=')), interpreter);
        std::string start = tn.second;
        LoopState * ls = new LoopState();
        ls->start = start.substr(0, start.find(':'));
        ls->dest = secondPart.substr(0, secondPart.find(','));
        ls->op = loopOperatorToLogicalOperator(line.substr(opFIndx, opLength));
        ls->step = secondPart.substr(assignIndex + 1);
        ls->loopBlockLine = interpreter->pcb->getLineNumber();
        PCBManager(interpreter).setLoop(ls);

        if(runFaster){
//            loopFaster(tn.first, initPart.substr(initPart.find('=') + 1));
        }
    }

    std::pair<size_t, size_t> findLoopOperator(const std::string &line)
    {
        std::vector<std::string> operators = {"up_to_eq", "up_to", "down_to_eq", "down_to"};
        size_t index, size;

        for(auto& op : operators){
            index = line.find(op);
            if(index != std::string::npos){
                size = op.length();
                goto ret;
            }
        }

    LABEL(ret)
        return {index, size};
    }

    std::string loopOperatorToLogicalOperator(const std::string &op)
    {
        std::map<std::string, std::string> ops =
            {{"up_to", "<"}, {"up_to_eq", "<="}, {"down_to", ">"}, {"down_to_eq", ">="}};

        return ops[op];
    }

    //    bool loopFaster(const std::string &type, const std::string initValue)
    //    {
    //        std::vector<std::string> instructions;
    //        std::string loopLine;
    //        size_t curlyBraces = 0;
    //        auto& pcb = interpreter->pcb;

    //        const auto loopCondition = [](auto i, const std::string& op, auto dest){
    //            if(op == ">")
    //                return i > dest;

    //            if(op == ">=")
    //                return i >= dest;

    //            if(op == "<")
    //                return i < dest;

    //            if(op == "<=")
    //                return i <= dest;

    //            return false;
    //        };

    //        while(true){
    //            std::getline(*interpreter->file, loopLine);
    //            pcb->nextLineNumber();
    //            instructions.push_back(loopLine);

    //            if(loopLine == "{")
    //            {
    //                curlyBraces++;
    //            }

    //            if(loopLine == "}")
    //            {
    //                curlyBraces--;
    //            }

    //            if(loopLine == "}" && curlyBraces == 0)
    //            {
    //                pcb->nextLineNumber();
    //                break;
    //            }
    //        }

    //        switch (interpreter->stringTypeToEnumType(type)) {
    //        case LLCore::DTS::Int:
    //        {
    //            LOOP(int, std::stoi)
    //            break;
    //        }
    //        case LLCore::DTS::LInt:
    //        {
    //            LOOP(long, std::stol)
    //            break;
    //        }
    //        case LLCore::DTS::LLInt: {
    //            LOOP(long long, std::stoll)
    //            break;
    //        }
    //        case LLCore::DTS::UInt: {
    //            LOOP(unsigned int, std::stoul)
    //            break;
    //        }
    //        case LLCore::DTS::F32: {
    //            LOOP(float, std::stof)
    //            break;
    //        }
    //        case LLCore::DTS::F64: {
    //            LOOP(double, std::stod)
    //            break;
    //        }
    //        case LLCore::DTS::InValid: return false;
    //        default: return false;
    //        }

    //        pcb->freeLoop();
    //        return true;
    //        //case DTS::InValid: throw std::runtime_error("invalid type on loop variable.");
    //        //default: throw std::runtime_error("invalid type on loop variable.");
    //    }

private:
    Core * interpreter;
};

#endif // LOOP_H
