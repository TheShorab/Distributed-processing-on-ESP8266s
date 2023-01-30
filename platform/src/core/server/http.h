#ifndef HTTP_H
#define HTTP_H

#include "../base.h"

#include <vector>
#include <queue>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <map>

namespace Platform
{
    namespace HTTP
    {
        struct ClientVitals
        {
            String ip;
            unsigned int id;
        };

        DECLARE(ESP8266WebServer, *server);
        DECLARE(std::queue<unsigned int>, *disconnected_children);
        std::map<String, ClientVitals> *children;

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME get_id_handler()
        {
            struct station_info *station_list = wifi_softap_get_station_info();
            while (station_list != NULL)
            {
                auto station_ip = IPAddress((&station_list->ip)->addr).toString().c_str();
                char station_mac[18] = {0};
                sprintf(station_mac, "%02X:%02X:%02X:%02X:%02X:%02X", MAC2STR(station_list->bssid));

                if (children->find(String(station_mac)) != children->end())
                {
                    if (!disconnected_children->empty())
                    {
                        size_t newID = disconnected_children->front();
                        disconnected_children->pop();
                        ClientVitals clv;
                        clv.id = newID;
                        clv.ip = server->client().localIP().toString();
                        children->insert({String(station_mac), clv});
                        server->send(200, "text/plain", String(newID));
                    }
                    else
                    {
                        size_t newID = children->size();
                        ClientVitals clv;
                        clv.id = newID;
                        clv.ip = server->client().localIP().toString();
                        children->insert({String(station_mac), clv});
                        server->send(200, "text/plain", String(newID));
                    }
                    break;
                }

                station_list = STAILQ_NEXT(station_list, next);
            }

            wifi_softap_free_station_info();
            PrintMessage(STR("New CPU(s) connected."));
            PrintMessage(STR("Available CPUs: "));

            int i = 0;
            for (auto &[key, value] : *children)
            {
                i++;
                Print(String(i) + "- " + "MAC: " + key + " IP: " + value.ip + " SSID: CPU-" + String(value.id));
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME disconnect_handler(const WiFiEventSoftAPModeStationDisconnected &event)
        {
            char station_mac[18] = {0};
            sprintf(station_mac, "%02X:%02X:%02X:%02X:%02X:%02X", MAC2STR(event.mac));
            auto &clv = children->at(String(station_mac));
            disconnected_children->push(clv.id);

            PrintMessage(STR("One CPU disconnected. MAC: ") + String(station_mac) + " ip: " + clv.ip + STR(" SSID: CPU-") + String(clv.id));
            PrintMessage(STR("Number of available CPUs: ") + String(children->size()));
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME event_handler(WiFiEvent ev)
        {
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME launch(IPAddress ip)
        {
            // register_cb
            children = new std::map<String, ClientVitals>();
            server = new ESP8266WebServer(ip, WEB_SERVER_PORT);
            server->on("get-id", HTTPMethod::HTTP_GET, get_id_handler);
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME request(String url)
        {
            HTTPClient http;
            String payload = "-"; // if first char is - it means error in request
            auto client = server->client();

            if (http.begin(client, url))
            {
                int http_code = http.GET();

                if (http_code > 0)
                {
                    DEBUG_PRINT("[HTTP] GET... code: " + http_code);

                    if (http_code == HTTP_CODE_OK)
                        payload = http.getString();
                }
                else
                {
                    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(http_code).c_str());
                }

                http.end();
            }
            else
            {
                Serial.println("[HTTP] Unable to connect to " + url);
            }

            return payload;
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME request_id(IPAddress ip)
        {
            return request(ip.toString() + "/get-id");
        }
    }
}

#endif