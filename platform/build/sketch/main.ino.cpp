#include <Arduino.h>
#line 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
#include "./core/server/server.h"
#include "./core/file-manager.h"
#include "./core/network-node.h"
#include "./core/flash-light.h"
#include "./interpreter/interpreter.h"

namespace Platform
{
    Platform::Modules::Interpreter interpreter;
}

#line 12 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
void setup();
#line 31 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
void loop();
#line 12 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
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
    Platform::Base::Data::file = SD.open(STR("./source.pt"), "r+");
    Platform::interpreter.initialize(&Platform::Base::Data::file);
}

void loop()
{
    if (!Platform::Base::Data::file)
    {
        Println(STR("Failed to open file for reading"));
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

