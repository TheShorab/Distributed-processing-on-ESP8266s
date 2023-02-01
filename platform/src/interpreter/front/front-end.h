#ifndef FRONTEND_H
#define FRONTEND_H

#include "../core/pcb-manager.h"
#include "../core/function.h"
#include "../core/if-else.h"
#include "../core/loop.h"
#include "../core/variable.h"
#include "../core/branch-to.h"

class FrontEnd : public Core
{

public:
    CONSTRUCTOR FrontEnd()
    {
    }

    DESTRUCTOR ~FrontEnd()
    {
        for (const auto &[key, value] : _functions)
        {
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

        for (auto &c : code)
        {
            if (c != '\n')
                line += c;
            else
            {
                lines.push_back(line);
                line.clear();
            }
        }

        for (auto &line : lines)
        {
            runOneCommand(line);
        }
    }

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME start()
    {
        LLCore::setRunning(true);
        run();
    }

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME resume()
    {
        start();
    }

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME runOneCommand(const std::string &line)
    {
        YIELD

        // DEBUG_PRINTLN(line);

        if (!LLCore::isRunning())
        {
            PRINT_FUNC_INFO;
            Println("Execution is blocked.");
            return;
        }

        const auto index = line.find(':');
        std::string keyword = "";
        std::string command = line;

        if (index == std::string::npos)
        {
            PCBManager(this).manager(keyword, command, true);
        }
        else
        {
            keyword = trim_copy(line.substr(0, index));
            command = trim_copy(line.substr(index + 1));
            PCBManager(this).manager(keyword, command, true);
        }

        if (pcb->isRunAllowed())
            runCommand(keyword, command);
    }

    FN_ATTRIBUTES(INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME addExternalVariable(const std::string &name, double value)
    {
        _values[name] = {"double", value};
    }

private:
    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME runCommand(const std::string &keyword, const std::string &command)
    {
        std::string copy = trim_copy(command);
        if (keyword.empty())
        {
            const auto index = copy.find('=');
            if (index != std::string::npos)
            {
                Variable(this).assignment(copy, index);
            }
        }
        else
        { // keyword
            auto e = E("Invalid Keyword Usage.");

            if (keyword == "///")
                return;

            switch (keywordStringTokeywordEnum(keyword))
            {
            case KWS::BranchTo:
                BranchTo(this).declare(command);
                break;
            case KWS::Declare:
                Variable(this).declare(command);
                break;
            case KWS::Else:
                break;
            case KWS::If:
                IfElseBlock(command, this);
                break;
            case KWS::Function:
                Function(this).declare(command);
                break;
            case KWS::Loop:
                Loop(this).run(command, false);
                break;
            case KWS::Return:
                Function(this).returnState(command);
                break;
            case KWS::Syscall:
                break;
            case KWS::Invoke:
                Function(this).invokeFunction(command);
                break;
            case KWS::Delete:
                Variable(this).deleteVariable(command);
                break;
            case KWS::Print:
                LLCore::print(command);
                break;
            case KWS::Paav:
                LLCore::printAll();
                break;
            case KWS::Dispatch:
                LLCore::printAll();
                break;
            case KWS::Use:
                LLCore::printAll();
                break;

            default:
                terminateExecution(e);
                break;
            }
        }
    }

/* ========================                 ========================
 * ======================== Windows VERSION ========================
 * ========================                 ======================== */
#ifndef ARDUINO

public:
    CONSTRUCTOR FrontEnd(const std::string &fileName) : Core(fileName), filename(fileName)
    {
        start();
    }

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME run()
    {
        file->close();
        file->open(filename, std::fstream::in);

        if (!file->is_open())
        {
            Print("file closed!\n");
            return;
        }

        std::string line;
        while (file->peek() != -1)
        {
            if (!running)
                break;

            std::getline(*this->file, line);

            runOneCommand(line);
        }
    }

private:
    std::string filename;

/* ========================                 ========================
 * ======================== ARDUINO VERSION ========================
 * ========================                 ======================== */
#else
    void run()
    {
        if (!file)
        {
            Print("file closed!\n");
            return;
        }

        std::string line;
        while (file->peek() != -1)
        {
            if (!running)
            {
                Println(STR("Execution Paused."));
                break;
            }

            for (char ch = (char)file->read(); file->available() && ch != '\n'; ch = (char)file->read())
                line.push_back(ch);

            runOneCommand(line);
        }
    }

public:
    void initFile(File *newFile)
    {
        file = newFile;
    }

#endif
};

#endif // FRONTEND_H
