//#include "interpreter/interpreter.h"

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

#include <iostream>
#include <QStack>
//#include "bitset"

int fn(){
    return 0b00000000000000000000000001100111;
}

int main(int argc, char *argv[])
{
    //    Interpreter(argc, argv);

    //    std::string path = "F:/UT/Term 1/Computer Achitecture/Project/Distributed-processing-on-ESP8266s/patrick-interpreter";
    //    std::string file = "/tests/04-functions/00-function.pt";
    //    Interpreter(path + file);

//    std::bitset<32> bin("01100111011011110111001001100111");

    int chars = 0b01100111011011110111001001100111;

    char * chs = (char*)&chars;
    chs[4] = 0;

    std::cout << chs << std::endl;

//    int char_ = 0b00000000000000000000000001100111;
////    std::cout << char_ << std::endl;

//    char ch = (char)fn();
//    std::cout << ch << std::endl;

    char ch_ = 's', ef = '\n';

    if(ch_ == ef)
    {

    }

    std::string s;
    for (char ch = (char)fn(); ch != ef; ch = (char)fn())
        s.push_back(ch);

}
















