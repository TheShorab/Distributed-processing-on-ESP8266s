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
        else
        {
            Println("");
#if defined(ARDUINO)
            Println("Patrick Language for ESP8266");
#else
            Println("Patrick Language for Windows");
#endif
            Println("Version -> 1.0.0");
            Println("============================");
            Println("\n");
            FrontEnd _interpreter(argv[1]);
            Println("");
            Println("Good Luck :)");
            Println("");
        }
    }

    void eventLoop(){
        FrontEnd _interpreter;
        std::string line;

        Println("");
        Println("Patrick Language for Windows");
        Println("Version -> 1.0.0");
        Println("============================");
        Println("commands ->");
        Println("\tExit  -> Ends execution.");
        Println("\tpaav: -> Prints All Allocated Values Till Now.");
        Print("\n\n");

        while(true)
        {
            Print(">> ");

            std::getline(std::cin, line);

            if(line == "exit") break;

            Print(line);
            _interpreter.runOneCommand(line);
        }

        Println("");
        Println("Good Luck :)");
        Println("");
    }
};

#endif // INTERPRETER_H
