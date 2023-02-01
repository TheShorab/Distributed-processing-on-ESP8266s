# 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino"
# 2 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 2
# 3 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 2
# 4 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 2
# 5 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 2
# 6 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 2

namespace Platform
{
    Platform::Modules::Interpreter interpreter;
}

void halt()
{
    Serial.println(Platform::to_printing_value(((reinterpret_cast<const __FlashStringHelper *>(
# 14 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "interpreter-test.ino" "." "14" "." "302" "\", \"aSM\", @progbits, 1 #"))) = (
# 14 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino"
   "going to halt state..."
# 14 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 3
   ); &__pstr__[0];}))
# 14 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino"
   )))));
    while (true)
    {
        delay(10000);
    }
}

void setup()
{
    delay(2000);
    Serial.begin(115200);

    Platform::FlashLight::initialize();

    bool sd_init_res = Platform::FileManager::initialize();

    if (sd_init_res)
    {
        Platform::Base::Data::ID = 0;
    }

    Platform::NetworkNode::initialize();
    Platform::FileManager::listDir("/", 0);
    Platform::Base::Data::file = SD.open(((reinterpret_cast<const __FlashStringHelper *>(
# 37 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 3
                                        (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "interpreter-test.ino" "." "37" "." "303" "\", \"aSM\", @progbits, 1 #"))) = (
# 37 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino"
                                        "/source.pt"
# 37 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 3
                                        ); &__pstr__[0];}))
# 37 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino"
                                        ))), "r+");
    Platform::interpreter.initialize(&Platform::Base::Data::file);
}

void loop()
{
    if (!Platform::Base::Data::file)
    {
        Serial.println(Platform::to_printing_value(((reinterpret_cast<const __FlashStringHelper *>(
# 45 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 3
       (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "interpreter-test.ino" "." "45" "." "304" "\", \"aSM\", @progbits, 1 #"))) = (
# 45 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino"
       "Failed to open file for reading"
# 45 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 3
       ); &__pstr__[0];}))
# 45 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino"
       )))));
        Serial.println(Platform::to_printing_value(((reinterpret_cast<const __FlashStringHelper *>(
# 46 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 3
       (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "interpreter-test.ino" "." "46" "." "305" "\", \"aSM\", @progbits, 1 #"))) = (
# 46 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino"
       "Is file available? "
# 46 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino" 3
       ); &__pstr__[0];}))
# 46 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\interpreter-test.ino"
       ))) + String(bool(Platform::Base::Data::file.available()))));
        halt();
    }
    else
    {
        Platform::Modules::Interpreter::setRunning(true);
    }

    while (Platform::Base::Data::file.available())
    {
        yield();
        Platform::interpreter.run_command(Platform::FileManager::readLine(Platform::Base::Data::file));
    }

    if (!Platform::Base::Data::file.available())
    {
        if (!Platform::Modules::Interpreter::isExited())
            Platform::interpreter.exit();

        halt();
    }
}
