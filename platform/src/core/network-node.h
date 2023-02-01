#ifndef NETWORK_H
#define NETWORK_H

#include "password-generator.h"
#include "server/server.h"
#include "base.h"

#include <ESP8266WiFi.h>

namespace Platform
{
    namespace NetworkNode
    {
        struct Node
        {
            String ssid;
            String password;
        };

        typedef std::function<String(IPAddress)> GetIDRequestFunction;

        FN_ATTRIBUTES(INLINE INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(IPAddress)
        FN_NAME parent_ip()
        {
            return WiFi.gatewayIP();
        }

        FN_ATTRIBUTES(INLINE INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(IPAddress)
        FN_NAME ap_ip()
        {
            return WiFi.softAPIP();
        }

        FN_ATTRIBUTES(INLINE INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(IPAddress)
        FN_NAME wifi_ip()
        {
            return WiFi.localIP();
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME connect_to_wifi(const Node &wifi)
        {
            Serial.print("connecting to Wifi:");
            Serial.println(wifi.ssid);
            WiFi.begin(wifi.ssid, wifi.password);

            while (WiFi.status() != WL_CONNECTED)
            {
                delay(500);
                Serial.print(".");
            }

            Serial.println("");
            Serial.println("WiFi connected");
            Serial.print("IP address: ");
            Serial.println(wifi_ip());
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME make_access_point(const Node &accessPoint)
        {
            WiFi.softAP(accessPoint.ssid, accessPoint.password);
            Serial.print("Access Point IP address: ");
            Serial.println(ap_ip());
            Serial.println("");
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(const Node)
        FN_NAME select_access_point()
        {
            int8_t networks = WiFi.scanNetworks();

            while (networks == 0)
            {
                delay(1000);
                networks = WiFi.scanNetworks();
            }

            return {"CPU-" + String(0), generatePassword(0)};
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(const Node)
        FN_NAME generate_access_point_info()
        {
            return {"CPU-" + String(Base::Data::ID), generatePassword(Base::Data::ID)};
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME assign_id(GetIDRequestFunction cb)
        {
            String new_id = cb(parent_ip());
            PrintMessage(STR("The Allocated Id is: ") + new_id);
            Base::Data::ID = new_id.toInt();
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME initialize()
        {
            delay(500);

            if (Base::Data::ID == 0)
            {
                WiFi.mode(WiFiMode::WIFI_AP);
                make_access_point(generate_access_point_info());
                Platform::Server::launch(NetworkNode::wifi_ip());
                WiFi.onSoftAPModeStationDisconnected(Platform::HTTP::disconnect_handler);
                // WiFi.onEvent([](WiFiEvent ev)
                //              {
                //                     Print(STR("WiFi event: "));
                //                     Println(String(ev)); });
                return;
            }

            WiFi.mode(WiFiMode::WIFI_STA);
            connect_to_wifi(select_access_point());
            Platform::Server::launch(NetworkNode::wifi_ip());
            assign_id(Platform::HTTP::request_id);

            delay(1500);
            return;
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME checkWiFiConnectivity(GetIDRequestFunction cb)
        {
            if (WiFi.isConnected())
                return;

            initialize();
        }
    }
}

#endif // NETWORK_H
