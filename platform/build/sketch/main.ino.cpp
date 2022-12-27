#include <Arduino.h>
#line 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
#include "./core/file-manager.h"
#include "./core/network.h"

#line 4 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
void setup();
#line 20 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
void loop();
#line 4 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
void setup()
{
    Serial.begin(115200);

    bool sd_init_res = Platform::FileManager::initialize();

    if (sd_init_res)
    {
        Platform::NetworkNode::ID = 0;
    }

    auto &server = Platform::NetworkNode::initialize();

    server
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

