#ifndef PCBMANAGER_H
#define PCBMANAGER_H

// #include "loop.h"
#include "core.h"
#include "if-else.h"
#include "variable.h"
#include "loop-state.h"

struct PCBManager
{

public:
    PCBManager(Core *interpreter) : interpreter(interpreter)
    {
    }

    void manager(const std::string &kw, const std::string &line, bool file = false)
    {
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
        auto &pcb = interpreter->pcb;
        pcb->setCurrentLine(line);

        if (!pcb->isWithinFunctionDeclaration())
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
                // pcb->scope.second++
                pcb->scope++;
                pcb->setWithinLoopBlock(true);
            }
        }

        if (line.find('{') != std::string::npos)
        {
            if (pcb->isWithinLoopBlock())
            {
                IfElseBlock(loop()->generateBoolExpression(), interpreter);
            }

            if (pcb->isWithinFunctionCall())
            {
                DEBUG_PRINT(line);
            }

            //            pcb->scope.first += 1;
            ++pcb->scope;
            pcb->curlyBraces()++;
        }

        if (!pcb->isRunAllowed() && !pcb->isWithinIfBlock() &&
            !pcb->isWithinElseBlock() && !pcb->isWithinFunctionDeclaration())
            pcb->freeScope({0, 0});

        if (line.find('}') != std::string::npos)
        {
            removeVariablesAllocatedInThisScope(interpreter);

            if (pcb->isWithinFunctionDeclaration() && (pcb->curlyBraces() - 1) == 0)
            {
                pcb->setWithinFunctionDeclaration(false);
                goto end;
            }

            if (pcb->isWithinFunctionCall())
            {
                pcb->setWithinFunctionCall(false);
                goTo(pcb->retAdd() + 1, interpreter);
                goto end;
            }

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

            if (pcb->isWithinElseBlock())
            {
                pcb->setWithinElseBlock(false);
                if (!pcb->isRunAllowed())
                {
                    pcb->freeScope(pcb->scope);
                }

                goto end;
            }

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
                    //                    pcb->scope.first--;
                    --pcb->scope;
                    removeVariablesAllocatedInThisScope(interpreter);
                    //                    pcb->scope.first++;
                    ++pcb->scope;
                    //                    ++pcb->scope; // becuace after end: will decrese again
                    //                    pcb->scope.second--;
                    pcb->scope--;
                    //                    setRunAllowed(true);
                    freeLoop();
                    pcb->freeScope(pcb->scope);
                    pcb->setWithinElseBlock(false);
                    pcb->setWithinIfBlock(false);
                }
            }

        end:
            //            pcb->scope.first--;
            --pcb->scope;
            pcb->curlyBraces()--;
        }
    }

    void goTo(size_t line, Core *interpreter)
    {
        interpreter->file->seekg(std::ios::beg);
        for (size_t i = 0; i < line - 1; ++i)
        {
            interpreter->file->ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        interpreter->pcb->setLineNumber(line);
    }

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

    void removeVariablesAllocatedInThisScope(Core *interpreter)
    {
        std::vector<std::string> forErase;
        for (const auto &[key, value] : interpreter->_values)
        {
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
