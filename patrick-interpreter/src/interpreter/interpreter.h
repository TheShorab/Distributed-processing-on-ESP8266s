#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "./front/front-end.h"

class Interpreter
{
public:
    Interpreter(const std::string& filename)
    {
        FrontEnd _interpreter(filename);
    }

    Interpreter(int argc, char *argv[])
    {
        if(argc <= 1) eventLoop();

        FrontEnd _interpreter(argv[1]);
    }

    void eventLoop(){
        FrontEnd _interpreter;
        std::string line;
        while(true)
        {
            std::cin >> line;

            if(line == "exit") break;

            _interpreter.runOneCommand(line);
        }
    }
};

#endif // INTERPRETER_H
