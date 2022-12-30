#include <Arduino.h>
#line 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
#include "./core/server/server.h"
#include "./core/file-manager.h"
#include "./core/network-node.h"
#include "./core/flash-light.h"

#line 6 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
void setup();
#line 30 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
void loop();
#line 6 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
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
    Platform::FileManager::readFile("/hello.txt");

    std::any a = 20;

    Print(std::any_cast<int>(a));
}

void loop()
{
}

