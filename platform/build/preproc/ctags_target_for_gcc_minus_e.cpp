# 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
# 2 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2
# 3 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2

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
