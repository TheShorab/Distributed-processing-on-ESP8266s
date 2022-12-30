#include "./core/server/server.h"
#include "./core/file-manager.h"
#include "./core/network-node.h"
#include "./core/flash-light.h"

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
}

void loop()
{
}
