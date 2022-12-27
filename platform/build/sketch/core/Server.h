#line 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\core\\Server.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <vector>
#include "PasswordGenerator.h"

namespace Platform
{
    namespace NetworkNode
    {
        ESP8266WebServer server(80);
        // IPAddress deviceIP;
        int8_t ID;
        std::vector<IPAddress> children;
        IPAddress parentIP() { return WiFi.gatewayIP(); };
        IPAddress AP_IP() { return WiFi.softAPIP(); }
        IPAddress WIFI_IP() { return WiFi.localIP(); };

        struct Node
        {
            String ssid;
            String password;
        };

        void connectToWIFI(const Node &wifi)
        {
            Serial.print("connecting to Wifi:");
            Serial.println(wifi.ssid);
            WiFi.begin(wifi.ssid, wifi.password);
            while (WiFi.status() != WL_CONNECTED)
            {
                delay(500);
                Serial.print(".");
            }

            // WIFI_IP = WiFi.localIP();
            Serial.println("");
            Serial.println("WiFi connected");
            Serial.print("IP address: ");
            Serial.println(WIFI_IP());
        }

        void makeAccessPoint(const Node &accessPoint)
        {
            WiFi.softAP(accessPoint.ssid, accessPoint.password);
            // IPAddress AP_IP = WiFi.softAPIP();

            Serial.print("Access Point IP address: ");
            Serial.println(AP_IP());
            Serial.println("");
        }

        const Node &selectAccessPoint()
        {
            int8_t max = 0;

            delay()
                int8_t networks = WiFi.scanNetworks();
            // while (networks == 0)
            // {
            //     delay(1000);
            //     networks = WiFi.scanNetworks();
            // }

            if (networks == 0)
            {
                ID = 0;
                return {"CPU-" + String(ID), generatePassword(ID)};
            }

            for (int8_t i = 0; i < networks; i++)
            {
                String network = WiFi.SSID(i);
                if (network.startsWith("CPU-"))
                {
                    int8_t number = network.substring(4).toInt();
                    if (number > max)
                        max = number;
                }
            }

            ID = max + 1;
            return {"CPU-" + String(ID), generatePassword(ID)};
        }

        const Node &generateAccessPointInfo()
        {
            return {"CPU-" + String(ID), generatePassword(ID)};
        }

        ESP8266WebServer &initialize(const Node &wifi, const Node &accessPoint)
        {
            delay(500);
            WiFi.mode(WIFI_AP_STA);

            // First Connects To NodeMcu-Active
            connectToWIFI(wifi);

            // First the name is NodeMcu-passive
            makeAccessPoint(accessPoint);

            delay(1500);
            return server;
        }

        /**
         * هر کدام از آی پی های اکسس پوینت یا وای فای اکین فرقی ندارن باهم از جهت درسترسی به سرور
         * فقط هرکس که کانکت شد بهمون باید آی پی اکسس پوینت رو بدیم بهش به عنوان آی پی پرنتش
         * هرکس هم بهش کانکت شدیم آی پی وای فایمون رو به عنوان آی پی چایلد بدیم بهش
         * توی سرور گزینه ی سند دیتا بزاریم
         * باید بدونیم کدوم نود به اینترنت دسترسی داره
         * هر نود باید رول خودش رو داشته باشه
         * توی تمام پکت ها باید شماره ی نود درخواست دهنده یا شماره ی نودی که ازش دیتا میخوایم وجود داشته باشه
         * مسیری که پکت طی کرده با توجه به شماره ی نود ها مشخص باشه
         * در هر پکت یک کامند وجود داشته باشه و نود های با رول های مشخص کامند های شناخته شده ای برای کل سیستم داشته باشن
         * نود تصمیم گیرنده مشخص باشه
         * نود ها باید بتونن ساعت و مدت زمان تایم آوت و .. رو از نود تصمیم گیرنده درخواست کنند.
         * نود تصمیم گیرنده باید بتونه بقیه ی نود ها رو به مدت مشخصی به خواب ببره ولی نمیتونه از خواب بیدار بکنه
         * هر نودی که بیدار شد خودشو به شبکه معرفی کنه
         * معرفی به شبکه شامل یک پکت میشه که دیتاش آیدی و رول هستش
         */

        void checkWiFiConnectivity()
        {
            if (WiFi.isConnected())
                return;

            initialize(selectAccessPoint(), generateAccessPointInfo());
        }
    }
}
