#ifndef NETWORK_H
#define NETWORK_H

#include "password-generator.h"

#include <ESP8266WiFi.h>
#include <vector>

namespace Platform
{
    namespace NetworkNode
    {
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

        void initialize()
        {
            delay(500);

            WiFi.mode(WIFI_AP_STA);

            if (ID == 0)
            {
                makeAccessPoint(generateAccessPointInfo());
                return;
            }

            // First Connects To NodeMcu-Active
            connectToWIFI(selectAccessPoint());

            // First the name is NodeMcu-passive
            makeAccessPoint(generateAccessPointInfo());

            delay(1500);
            return;
        }

        void checkWiFiConnectivity()
        {
            if (WiFi.isConnected())
                return;

            initialize();
        }
    }
}

#endif // NETWORK_H
