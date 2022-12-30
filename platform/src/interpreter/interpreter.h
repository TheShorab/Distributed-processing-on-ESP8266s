#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "./front/front-end.h"

namespace Platform
{
    namespace Modules
    {
        class Interpreter
        {

        public:
            Interpreter(int argc, char *argv[])
            {
                if (argc <= 1)
                    eventLoop();
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
                    _interpreter = FrontEnd(argv[1]);
                    Println("");
                    Println("Good Luck :)");
                    Println("");
                }
            }

#ifndef ARDUINO
            Interpreter(const std::string &filename)
            {
                Println("");
                Println("Patrick Language for Windows");
                Println("Version -> 1.0.0");
                Println("============================");
                Println("\n");
                FrontEnd _interpreter(filename);
                Println("");
                Println("Good Luck :)");
                Println("");
            }

            void eventLoop()
            {
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

                while (true)
                {
                    Print(">> ");

                    std::getline(std::cin, line);

                    if (line == "exit")
                        break;

                    Print(line);
                    _interpreter.runOneCommand(line);
                }

                Println("");
                Println("Good Luck :)");
                Println("");
            }
#else
            Interpreter(File *file)
            {
                Println("");
                Println("Patrick Language for ESP8266");
                Println("Version -> 1.0.0");
                Println("============================");
                Println("\n");
                _interpreter.initFile(file);
                _interpreter.start();
                Println("");
                Println("Good Luck :)");
                Println("");
            }

            Interpreter()
            {
            }

            Interpreter &initialize(File *file)
            {
                Println("");
                Println("Patrick Language for ESP8266");
                Println("Version -> 1.0.0");
                Println("============================");
                Println("\n");
                _interpreter.initFile(file);
                return *this;
            }

            void start()
            {
                _interpreter.start();
            }

            void pause()
            {
                _interpreter.pause();
            }

            void resume()
            {
                _interpreter.resume();
            }

            void exit()
            {
                Println("");
                Println("Good Luck :)");
                Println("");
            }

            void eventLoop()
            {
                Platform::Base::terminate_execution("You CAN NOT use Patrick RealTime Interpret mode on Micro controllers.");
            }

            void run_command(const std::string &line)
            {
                return _interpreter.runOneCommand(line);
            }

        private:
            FrontEnd _interpreter;
#endif
        };
    }
}

#endif // INTERPRETER_H
