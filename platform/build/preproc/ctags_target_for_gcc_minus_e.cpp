# 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
# 2 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2
# 3 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2
# 4 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2
# 5 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2
# 6 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2

namespace Platform
{
    Platform::Modules::Interpreter interpreter;
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
    Platform::Base::Data::file = SD.open(((reinterpret_cast<const __FlashStringHelper *>(
# 27 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 3
                                        (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "main.ino" "." "27" "." "256" "\", \"aSM\", @progbits, 1 #"))) = (
# 27 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
                                        "./source.pt"
# 27 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 3
                                        ); &__pstr__[0];}))
# 27 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
                                        ))), "r+");
    Platform::interpreter.initialize(&Platform::Base::Data::file);
}

void loop()
{
    if (!Platform::Base::Data::file)
    {
        Serial.println(Platform::to_printing_value(((reinterpret_cast<const __FlashStringHelper *>(
# 35 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 3
       (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "main.ino" "." "35" "." "257" "\", \"aSM\", @progbits, 1 #"))) = (
# 35 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
       "Failed to open file for reading"
# 35 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 3
       ); &__pstr__[0];}))
# 35 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
       )))));
        return;
    }

    while (Platform::Base::Data::file.available())
    {
        Platform::interpreter.run_command(Platform::FileManager::readLine(Platform::Base::Data::file));
    }

    if (!Platform::Base::Data::file.available())
    {
        Platform::interpreter.exit();
    }
}
