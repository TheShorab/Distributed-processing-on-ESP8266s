# 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino"
# 2 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino" 2
# 3 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino" 2
# 4 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino" 2
# 5 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino" 2
# 6 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino" 2

namespace Platform
{
    Platform::Modules::Interpreter interpreter;
}

void halt()
{
    Serial.println(Platform::to_printing_value(((reinterpret_cast<const __FlashStringHelper *>(
# 14 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino" 3
   (__extension__({static const char __pstr__[] __attribute__((__aligned__(4))) __attribute__((section( "\".irom0.pstr." "network-test.ino" "." "14" "." "268" "\", \"aSM\", @progbits, 1 #"))) = (
# 14 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino"
   "going to halt state..."
# 14 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino" 3
   ); &__pstr__[0];}))
# 14 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\network-test.ino"
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
}

void loop()
{
    if (Platform::Base::Data::ID == 0)
        Platform::HTTP::server->handleClient();
    else
    {
        // Platform::HTTP::request();
    }
}
