#include "tests.h"
#include "interpreter/interpreter.h"

class ProcessTimer {
public:
    ProcessTimer()
    {
        _start = std::chrono::steady_clock::now();
    }
    ~ProcessTimer()
    {
        std::cout << "\n\n duration of process : " <<
            std::chrono::nanoseconds(std::chrono::steady_clock::now() - _start ).count() / 10e8 <<
            " second(s)\n\n";

    }

private:
    std::chrono::steady_clock::time_point _start ;
};

int main(int argc, char *argv[])
{
    //    Interpreter(argc, argv);

    std::string path = "F:/UT/Term 1/Computer Achitecture/Project/Distributed-processing-on-ESP8266s/patrick-interpreter";
    std::string file = "/tests/04-functions/00-function.pt";
    Interpreter(path + file);
}
















