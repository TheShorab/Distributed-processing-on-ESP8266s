#include "./core/server/server.h"
#include "./core/file-manager.h"
#include "./core/network-node.h"
#include "./core/flash-light.h"
#include "./interpreter/interpreter.h"

namespace Platform
{
    Platform::Modules::Interpreter interpreter;
}

void halt()
{
    Println(STR("going to halt state..."));
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
        Platform::FileManager::listDir("/", 0);
        Platform::Base::Data::file = SD.open(STR("/source.pt"), "r+");
        Platform::interpreter.initialize(&Platform::Base::Data::file);
    }

    Platform::NetworkNode::initialize();
}

void loop()
{
    Platform::HTTP::server->handleClient();
}
