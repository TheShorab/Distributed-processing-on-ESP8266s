#ifndef LOOP_STATE_H
#define LOOP_STATE_H

#include <string>

struct LoopState{
    int loopBlockLine;
    std::string dest;
    std::string start;
    std::string op;
    std::string step;

    //    void increaseStart(Core* interpreter)
    //    {
    //        interpreter->runCommand(start + " = " + start + " + " + step);
    //    }

    std::string generateIncreaseStartExpression()
    {
        return start + " = " + start + " + " + step;
    }

    std::string generateBoolExpression()
    {
        return start + " " + op + " " + dest;
    }
};

#endif // LOOP_STATE_H
