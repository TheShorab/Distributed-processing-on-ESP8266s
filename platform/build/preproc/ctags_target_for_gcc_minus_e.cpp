# 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino"
# 2 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2
# 3 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2
# 4 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2
# 5 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\main.ino" 2

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

    Serial.println(std::any_cast<int>(a));
}

void loop()
{
}
