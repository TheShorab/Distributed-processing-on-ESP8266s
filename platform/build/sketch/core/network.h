#line 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\core\\network.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <vector>
#include "password-generator.h"

namespace Platform
{
    namespace NetworkNode
    {
        ESP8266WebServer server(80);
        // IPAddress deviceIP;
        int8_t ID = -1;
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

        const Node selectAccessPoint()
        {
            int8_t networks = WiFi.scanNetworks();

            while (networks == 0)
            {
                delay(1000);
                networks = WiFi.scanNetworks();
            }

            return {"CPU-" + String(0), generatePassword(0)};
        }

        const Node generateAccessPointInfo()
        {
            if (ID == 0)
                return {"CPU-" + String(ID), generatePassword(ID)};

            // connect request id from root parentIP()
        }

        ESP8266WebServer &initialize()
        {
            delay(500);

            WiFi.mode(WIFI_AP_STA);

            if (ID == 0)
            {
                makeAccessPoint(generateAccessPointInfo());
                return server;
            }

            // First Connects To NodeMcu-Active
            connectToWIFI(selectAccessPoint());

            // First the name is NodeMcu-passive
            makeAccessPoint(generateAccessPointInfo());

            delay(1500);
            return server;
        }

        void checkWiFiConnectivity()
        {
            if (WiFi.isConnected())
                return;

            initialize();
        }
    }
}
