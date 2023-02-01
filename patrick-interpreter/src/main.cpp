//#include "interpreter/interpreter.h"
#include "../../platform/src/interpreter/interpreter.h"

//class ProcessTimer {
//public:
//    ProcessTimer()
//    {
//        _start = std::chrono::steady_clock::now();
//    }
//    ~ProcessTimer()
//    {
//        std::cout << "\n\n duration of process : " <<
//            std::chrono::nanoseconds(std::chrono::steady_clock::now() - _start ).count() / 10e8 <<
//            " second(s)\n\n";

//    }

//private:
//    std::chrono::steady_clock::time_point _start ;
//};

//std::cout << chars << " " << sizeof(int) << std::endl;

//std::cout << (short)'g' << " "
//          << (short)'o' << " "
//          << (short)'r' << " "
//          << (short)'g' << std::endl;

//std::cout << std::bitset<8>((short)'g').to_string() << " "
//          << std::bitset<8>((short)'o').to_string() << " "
//          << std::bitset<8>((short)'r').to_string() << " "
//          << std::bitset<8>((short)'g').to_string() << std::endl;

int main(int argc, char *argv[])
{
    Platform::Modules::Interpreter
        interpreter("F:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\patrick-interpreter\\tests\\04-functions\\00-function.pt");

//    Platform::Modules::Interpreter interpreter(argc, argv);

}


