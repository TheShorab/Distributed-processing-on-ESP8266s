#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "./front/front-end.h"

namespace Platform
{
    namespace Modules
    {
        class Interpreter
        {

/* ========================                 ========================
 * ======================== Windows VERSION ========================
 * ========================                 ======================== */

#ifndef ARDUINO

        public:

            CONSTRUCTOR Interpreter(int argc, char *argv[])
            {
                if (argc <= 1)
                    eventLoop();
                else
                {
                    welcome();
                    _interpreter = FrontEnd(argv[1]);
                    Println("");
                    Println("Good Luck :)");
                    Println("");
                }
            }

            CONSTRUCTOR Interpreter(const std::string &filename)
            {
                welcome();
                FrontEnd _interpreter(filename);
                Println("");
                Println("Good Luck :)");
                Println("");
            }

            FN_ATTRIBUTES(INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME eventLoop()
            {
                FrontEnd _interpreter;
                std::string line;

                welcome();
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

                exit();
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME welcome()
            {
                Println("");
                Println("Patrick Language for Windows");
                Println("Version -> 1.0.0");
                Println("============================");
                Println("\n");
            }

/* ========================                 ========================
 * ======================== ARDUINO VERSION ========================
 * ========================                 ======================== */

#else
public:
            CONSTRUCTOR Interpreter()
            {
            }

            CONSTRUCTOR Interpreter(File *file)
            {
                welcome();
                _interpreter.initFile(file);
                _interpreter.start();
                Println("");
                Println("Good Luck :)");
                Println("");
            }

            FN_ATTRIBUTES(INDEPENDENT)
            FN_RETURN_TYPE(Interpreter)
            FN_NAME &initialize(File *file)
            {
                welcome();
                _interpreter.initFile(file);
                return *this;
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME eventLoop()
            {
                Platform::Base::terminate_execution("You CAN NOT use Patrick RealTime Interpret mode on Micro controllers.");
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME run_command(const std::string &line)
            {
                return _interpreter.runOneCommand(line);
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME welcome()
            {
                Println("");
                Println("Patrick Language for ESP8266");
                Println("Version -> 1.0.0");
                Println("============================");
                Println("\n");
            }

#endif

            FN_ATTRIBUTES(INLINE INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME start()
            {
                _interpreter.start();
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME pause()
            {
                _interpreter.pause();
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME resume()
            {
                _interpreter.resume();
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT static)
            FN_RETURN_TYPE(bool)
            FN_NAME isRunning()
            {
                return LLCore::isRunning();
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT static)
            FN_RETURN_TYPE(void)
            FN_NAME setRunning(bool state)
            {
                LLCore::setRunning(state);
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME exit()
            {
                Println("");
                Println("Good Luck :)");
                Println("");
                LLCore::exit();
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT static)
            FN_RETURN_TYPE(bool)
            FN_NAME isExited()
            {
                return LLCore::isExited();
            }

            FN_ATTRIBUTES(INLINE INDEPENDENT)
            FN_RETURN_TYPE(void)
            FN_NAME addExternalVariable(const std::string& name, double value)
            {
                _interpreter.addExternalVariable(name, value);
            }

        private:
            FrontEnd _interpreter;
        };
    }
}

#endif // INTERPRETER_H
