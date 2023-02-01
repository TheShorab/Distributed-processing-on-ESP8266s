#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "core.h"
#include "pcb-manager.h"

#ifdef ARDUINO
#include "../../core/file-manager.h"
#include "../../core/server/http.h"
#endif

class Dispatcher
{
public:
    /* ========================                 ========================
     * ======================== ARDUINO VERSION ========================
     * ========================                 ======================== */

#ifdef ARDUINO
    CONSTRUCTOR Dispatcher(Core *core, const std::string &command) : _returnLineNumber(core->pcb->getLineNumber()), _interpreter(core)
    {
        auto &pcb = core->pcb;
        auto threads = this->threads(command);

        for (auto &thread : threads)
        {
            if (!threadExists(thread))
                LLCore::terminateExecution(E("Invalid Branch name!"));

            dispatch(getThread(thread));
        }
    }

private:
    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME dispatch(BranchType *branch)
    {
        auto &pcb = core->pcb;
        auto manager = PCBManager(_interpreter);
        manager.goTo(branch->start, _interpreter);
        auto mac = Platform::HTTP::children_ref.at(branch->destination);
        auto cpu = Platform::HTTP::children->at(mac);
        String response = Platform::HTTP::start_branch(cpu.ip);

        if (response[0] == '+')
        {
            for (unsigned int i = branch->start; i <= branch->end; i++)
            {
                std::string line = Platform::FileManager::readLine(*_interpreter->file);
                Platform::HTTP::branch_data(cpu.ip);
            }

            Platform::HTTP::end_branch(cpu.ip);
        }
        else
        {
            PrintCommand(STR("CPU ID = ") + String(cpu.id) + STR(" CPU IP = ") + cpu.ip);
            LLCore::terminateExecution(E("Destination CPU is in use! Branch Failed"))
        }
    }

    /* ========================                 ========================
     * ======================== Windows VERSION ========================
     * ========================                 ======================== */

#else
    CONSTRUCTOR Dispatcher(Core *core, const std::string &command) : _returnLineNumber(core->pcb->getLineNumber()), _interpreter(core)
    {
        auto &pcb = core->pcb;
        LLCore::terminateExecution(E("Do Not use branches on Windows!"));
    }

private:
    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(void)
    FN_NAME dispatch(MAYBE_UNUSED BranchType *branch)
    {
    }

#endif

private:
    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(std::vector<std::string>)
    FN_NAME threads(const std::string &command)
    {
        std::vector<std::string> threads;
        std::string buffer;

        for (auto &ch : command)
        {
            if (ch != ' ')
            {
                if (ch == ',')
                {
                    threads.push_back(buffer);
                    buffer.clear();
                }
                else
                {
                    buffer.push_back(ch);
                }
            }
        }

        if (!buffer.empty())
            threads.push_back(buffer);

        return threads;
    }

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(bool)
    FN_NAME threadExists(const std::string &thread)
    {
        return _interpreter->_branches.find(thread) != _interpreter->_branches.end();
    }

    FN_ATTRIBUTES(INLINE INDEPENDENT)
    FN_RETURN_TYPE(auto)
    FN_NAME getThread(const std::string &thread)
    {
        return _interpreter->_branches[thread];
    }

    Scope::ScopeSize _returnLineNumber;
    Core *_interpreter;
};

#endif // DISPATCHER_H
