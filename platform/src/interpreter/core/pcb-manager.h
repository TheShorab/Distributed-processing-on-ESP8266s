#ifndef PCBMANAGER_H
#define PCBMANAGER_H

// #include "loop.h"
#include "core.h"
#include "if-else.h"
#include "variable.h"
#include "loop-state.h"
#include "branch-to-state.h"

struct PCBManager
{

public:
    PCBManager(Core *interpreter) : interpreter(interpreter)
    {
    }

    void manager(const std::string &kw, const std::string &line, bool file = false)
    {
        YIELD
        if (!file)
        {
            interpreter->pcb->nextLineNumber();
            scopeManager(kw, line);
        }
        else
        {
            goTo(interpreter->pcb->nextLineNumber(), interpreter);
            scopeManager(kw, line);
        }
    }

    void scopeManager(const std::string &kw, const std::string &line)
    {
        YIELD

        auto &pcb = interpreter->pcb;
        pcb->setCurrentLine(line);

        if (!pcb->isWithinFunctionDeclaration() && !pcb->isWithinBranchBlock())
        {
            if (kw == "ivoke")
            {
                pcb->setWithinFunctionCall(true);
            }

            if (kw == "if")
            {
                pcb->setWithinIfBlock(true);
            }

            if (kw == "else")
            {
                pcb->setWithinElseBlock(true);
            }

            if (kw == "loop")
            {
                pcb->setWithinLoopBlock(true);
            }
        }

        YIELD
        if (line.find('{') != std::string::npos)
        {
            if (pcb->isWithinLoopBlock())
            {
                IfElseBlock(loop()->generateBoolExpression(), interpreter);
            }

            ++pcb->scope; // 1.1
            pcb->curlyBraces()++;
        }

        YIELD
        if (!pcb->isRunAllowed() && !pcb->isWithinIfBlock() &&
            !pcb->isWithinElseBlock() && !pcb->isWithinFunctionDeclaration() && !pcb->isWithinBranchBlock())
            pcb->freeScope({0, 0});

        YIELD
        if (line.find('}') != std::string::npos)
        {
            removeVariablesAllocatedInThisScope(interpreter); // 1.1

            YIELD
            if (pcb->isWithinFunctionDeclaration())
            {
                if ((pcb->curlyBraces() - 1) == 0)
                    pcb->setWithinFunctionDeclaration(false);

                goto end;
            }

            if (pcb->isWithinBranchBlock())
            {
                branch()->end = pcb->getLineNumber() - 1;
                pcb->setWithinBranchBlock(false);
                goto end;
            }

            YIELD
            if (pcb->isWithinFunctionCall() && (pcb->curlyBraces() - 1) == 0)
            {
                pcb->setWithinFunctionCall(false);
                goTo(pcb->retAdd() + 1, interpreter);
                goto end;
            }

            YIELD
            if (pcb->isWithinIfBlock())
            {
                pcb->setWithinIfBlock(false);
                if (pcb->isRunAllowed())
                {
                    pcb->blockThisScope();
                }
                else
                    pcb->freeScope(pcb->scope);

                goto end;
            }

            YIELD
            if (pcb->isWithinElseBlock())
            {
                pcb->setWithinElseBlock(false);
                if (!pcb->isRunAllowed())
                {
                    pcb->freeScope(pcb->scope);
                }

                goto end;
            }

            YIELD
            if (pcb->isWithinLoopBlock())
            {
                Variable(interpreter).assignment(loop()->generateIncreaseStartExpression(), 0, true);
                IfElseBlock(loop()->generateBoolExpression(), interpreter);
                if (pcb->isRunAllowedOnNextBlock())
                {
                    goTo(loop()->loopBlockLine, interpreter);
                }
                else
                {
                    --pcb->scope;
                    removeVariablesAllocatedInThisScope(interpreter); // 0.1
                    ++pcb->scope;                                     // 1.1
                    pcb->scope--;                                     // 1.0

                    freeLoop();
                    pcb->freeScope(pcb->scope);
                    pcb->setWithinElseBlock(false);
                    pcb->setWithinIfBlock(false);
                }
            }

        end:
            --pcb->scope; // 0.0
            pcb->curlyBraces()--;
        }
    }

#if defined(ARDUINO)
    void goTo(size_t line, Core *interpreter)
    {
        size_t seek = 0;
        interpreter->file->seek(std::ios::beg);

        for (size_t i = 0; i < line - 1; ++i)
        {
            YIELD

            for (char ch = (char)interpreter->file->read();
                 interpreter->file->available() && ch != '\n';
                 ch = (char)interpreter->file->read())
            {
                YIELD
            }

            // interpreter->file->read();
        }

        // interpreter->file->seek(seek);
        interpreter->pcb->setLineNumber(line);
    }
#else

    void goTo(size_t line, Core *interpreter)
    {
        interpreter->file->seekg(std::ios::beg);
        for (size_t i = 0; i < line - 1; ++i)
        {
            interpreter->file->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        interpreter->pcb->setLineNumber(line);
    }

#endif

    void setLoop(LoopState *loop)
    {
        interpreter->pcb->_loop = loop;
    }

    LoopState *loop()
    {
        return static_cast<LoopState *>(interpreter->pcb->_loop);
    }

    void freeLoop()
    {
        free(interpreter->pcb->_loop);
        interpreter->pcb->_loop = nullptr;
    }

    void setBranch(const std::string &branch)
    {
        interpreter->pcb->_lastBranch = branch;
    }

    BranchType *branch()
    {
        return static_cast<BranchType *>(interpreter->_branches[interpreter->pcb->_lastBranch]);
    }

    //    void freeBranch()
    //    {
    //        free(interpreter->pcb->_branch);
    //        interpreter->pcb->_branch = nullptr;
    //    }

    void removeVariablesAllocatedInThisScope(Core *interpreter)
    {
        YIELD
        std::vector<std::string> forErase;
        for (const auto &[key, value] : interpreter->_values)
        {
            YIELD
            auto const scp = interpreter->pcb->scopeFromName(key);

            if (scp.first == interpreter->pcb->scope.first &&
                scp.second == interpreter->pcb->scope.second)
                forErase.push_back(key);
        }

        for (const auto &key : forErase)
            interpreter->_values.erase(key);
    }

private:
    Core *interpreter;
};

#endif // PCBMANAGER_H
