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
    }

    Platform::NetworkNode::initialize();
    Platform::FileManager::listDir("/", 0);
    Platform::Base::Data::file = SD.open(STR("/source.pt"), "r+");
    Platform::interpreter.initialize(&Platform::Base::Data::file);
}

void loop()
{
    if (!Platform::Base::Data::file)
    {
        Println(STR("Failed to open file for reading"));
        Println(STR("Is file available? ") + String(bool(Platform::Base::Data::file.available())));
        halt();
    }
    else
    {
        Platform::Modules::Interpreter::setRunning(true);
    }

    while (Platform::Base::Data::file.available())
    {
        YIELD
        Platform::interpreter.run_command(Platform::FileManager::readLine(Platform::Base::Data::file));
    }

    if (!Platform::Base::Data::file.available())
    {
        if (!Platform::Modules::Interpreter::isExited())
            Platform::interpreter.exit();

        halt();
    }
}
