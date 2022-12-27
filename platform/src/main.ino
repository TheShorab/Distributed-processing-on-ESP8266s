#include "./core/file-manager.h"
#include "./core/network.h"

void setup()
{
    Serial.begin(115200);

    bool sd_init_res = Platform::FileManager::initialize();

    if (sd_init_res)
    {
        Platform::NetworkNode::ID = 0;
    }

    auto &server = Platform::NetworkNode::initialize();

    server;
}

void loop()
{
}

// void setup() {
//   pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
// }

// void loop() {
//   digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on by making the voltage LOW
//   delay(1000);                      // Wait for a second
//   digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
//   delay(2000);                      // Wait for two seconds
// }
