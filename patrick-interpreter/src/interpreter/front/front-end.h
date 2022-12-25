#ifndef FRONTEND_H
#define FRONTEND_H

#include "../core/pcb-manager.h"
#include "../core/function.h"
#include "../core/if-else.h"
#include "../core/loop.h"
#include "../core/variable.h"

class FrontEnd : Core
{

public:
    FrontEnd()
    {}

    FrontEnd(const std::string &fileName) :  Core(fileName)
    {
        file->close();
        file->open(fileName, std::fstream::in);

        if(!file->is_open()){
            std::cout << "file closed!\n";
            return;
        }

        std::string line;
        while(file->peek() != -1){
            std::getline(*this->file, line);

            runOneCommand(line);
        }
    }

    ~FrontEnd()
    {
        for(const auto &[key, value]: _functions){
            delete value;
        }

        delete pcb;
        delete file;
    }

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME runMultipleCommands(const std::string &code)
    {
        std::vector<std::string> lines;
        std::string line;

        for(auto& c : code){
            if(c != '\n')
                line += c;
            else {
                lines.push_back(line);
                line.clear();
            }
        }

        for(auto& line : lines){
            runOneCommand(line);
        }
    }

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME runOneCommand(const std::string &line)
    {
        DEBUG_PRINT(line);
        const auto index = line.find(':');
        std::string keyword = "";
        std::string command = line;

        if(index == std::string::npos)
        {
            PCBManager(this).manager(keyword, command, true);
        }
        else
        {
            keyword = trim_copy(line.substr(0, index));
            command = trim_copy(line.substr(index + 1));
            PCBManager(this).manager(keyword, command, true);
        }

        if(pcb->isRunAllowed()) runCommand(keyword, command);
    }

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME runCommand(const std::string &keyword, const std::string &command)
    {
//        DEBUG_PRINT(keyword + command);

        std::string copy = trim_copy(command);

        if(keyword.empty()){

            const auto index = copy.find('=');
            if(index != std::string::npos)
            {
                Variable(this).assignment(copy, index);
            }

        } else { //keyword
            auto e = E("Invalid Keyword Usage.", runCommand);

            if(keyword == "///") return;

            switch (keywordStringTokeywordEnum(keyword)) {
            case KWS::BranchTo: break;
            case KWS::Declare: Variable(this).declare(command); break;
            case KWS::Else: break;
            case KWS::If: IfElseBlock(command, this); break;
            case KWS::Function: Function(this).declare(command); break;
            case KWS::Loop: Loop(this).run(command, false); break;
            case KWS::Return: Function(this).returnState(command); break;
            case KWS::Syscall: break;
            case KWS::Invoke: Function(this).invokeFunction(command); break;
            case KWS::Delete: Variable(this).deleteVariable(command); break;
            case KWS::Print: LLCore::print(command);

            default: terminateExecution(e); break;
            }
        }

        return;
    }
};


#endif // FRONTEND_H
