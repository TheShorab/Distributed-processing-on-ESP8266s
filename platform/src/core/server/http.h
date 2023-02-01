#ifndef HTTP_H
#define HTTP_H

#include "../base.h"

#include <vector>
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
        std::map<String, ClientVitals> *children;
        std::vector<String> children_ref;

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME root_handler()
        {
            return server->send(200, "text/plain", STR("Welcome to out Multi-Core CPU."));
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME get_id_handler_v1()
        {
            Println("");
            PrintMessage(STR("==========================================="));

            struct station_info *station_list = wifi_softap_get_station_info();
            size_t CPUS = children->size();
            String mac;
            String id = "-";

            PrintMessage(STR("New CPU is connecting..."));
            PrintMessage(STR("MAC Address: ") + mac + STR(" IP Address: ") + server->client().remoteIP().toString());

            while (station_list != NULL)
            {
                auto station_ip = IPAddress((&station_list->ip)->addr).toString().c_str();
                char station_mac[18] = {0};
                sprintf(station_mac, "%02X:%02X:%02X:%02X:%02X:%02X", MAC2STR(station_list->bssid));
                mac = String(station_mac);

                if (children->find(mac) == children->end())
                {
                    size_t newID = children->size() + 1;
                    ClientVitals clv;
                    clv.id = newID;
                    clv.ip = server->client().remoteIP().toString();
                    children->insert({mac, clv});
                    String newID_s = String(newID);
                    server->send(200, "text/plain", newID_s);
                    PrintMessage(STR("New CPU(s) connected."));
                    break;
                }
                else
                {
                    auto &child = children->at(mac);
                    PrintMessage(STR("Remote IP: ") + server->client().remoteIP().toString() + STR(" Child IP: ") + child.ip);

                    if (child.ip == server->client().remoteIP().toString())
                        id = String(child.id);
                }

                station_list = STAILQ_NEXT(station_list, next);
            }

            wifi_softap_free_station_info();

            if (CPUS == children->size())
            {
                PrintMessage(STR("New CPU tried to connect but its mac is already exists."));
                if (id[0] == '-')
                    server->send(400, "text/plain", "You are already registered");
                else
                    server->send(200, "text/plain", id);
            }

            PrintMessage(STR("Available CPUs: "));
            int i = 0;
            for (auto &[key, value] : *children)
            {
                i++;
                PrintMessage(String(i) + "- " + "MAC: " + key + " IP: " + value.ip + " SSID: CPU-" + String(value.id));
            }

            PrintMessage(STR("==========================================="));
            Println("");
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME get_id_handler_v2()
        {
            Println("");
            PrintMessage(STR("==========================================="));

            if (!server->hasArg("plain"))
            {

                server->send(400, "text/plain", "Body not received");
                return;
            }

            String mac = server->arg("plain");
            String id = "-";
            String client_ip = server->client().remoteIP().toString();

            PrintMessage(STR("New CPU is connecting..."));
            PrintMessage(STR("MAC Address: ") + mac + STR(" IP Address: ") + client_ip);

            if (children->find(mac) == children->end())
            {
                size_t newID = children->size() + 1;
                ClientVitals clv;
                clv.id = newID;
                clv.ip = client_ip;
                children->insert({mac, clv});
                String newID_s = String(newID);
                children_ref.push_back(mac);
                server->send(200, "text/plain", newID_s);
                PrintMessage(STR("New CPU(s) connected."));
            }
            else
            {
                auto &child = children->at(mac);
                PrintMessage(STR("Remote IP: ") + server->client().remoteIP().toString() + STR(" Child IP: ") + child.ip);
                id = String(child.id);

                if (id[0] == '-')
                    server->send(400, "text/plain", "an error has been happened.");
                else
                    server->send(200, "text/plain", id);
            }

            PrintMessage(STR("Available CPUs: "));
            int i = 0;
            for (auto &[key, value] : *children)
            {
                i++;
                PrintMessage(String(i) + "- " + "MAC: " + key + " IP: " + value.ip + " SSID: CPU-" + String(value.id));
            }

            PrintMessage(STR("==========================================="));
            Println("");
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME disconnect_handler(const WiFiEventSoftAPModeStationDisconnected &event)
        {
            char station_mac[18] = {0};
            sprintf(station_mac, "%02X:%02X:%02X:%02X:%02X:%02X", MAC2STR(event.mac));
            auto &clv = children->at(String(station_mac));

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
        FN_NAME start_branch_handler()
        {
            Println("");
            PrintMessage(STR("==========================================="));

            if (Platform::Base::Branch::isOnBranch)
            {
                PrintMessage(STR("CPU is in use!"));
                server->send(200, "text/plain", "-");
            }

            PrintMessage(STR("Preparing to execute a branch!"));
            Platform::Base::Branch::isOnBranch = true;
            server->send(200, "text/plain", "+");

            PrintMessage(STR("==========================================="));
            Println("");
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(void)
        FN_NAME branch_data_handler()
        {
            Println("");
            PrintMessage(STR("==========================================="));

            if (!server->hasArg("plain"))
            {

                server->send(400, "text/plain", "Body not received");
                return;
            }

            std::string line = server->arg("plain").c_str();

            // if(ESP.getFreeHeap())

            Platform::Base::Branch::branchCode.push_back(line);
            server->send(200, "text/plain", "+");

            PrintMessage(STR("==========================================="));
            Println("");
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME end_branch_handler()
        {
            Println("");
            PrintMessage(STR("==========================================="));

            Platform::Base::Branch::isReadyToExecute = true;
            server->send(200, "text/plain", "+");

            PrintMessage(STR("==========================================="));
            Println("");
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME server_init(IPAddress ip)
        {
            server = new ESP8266WebServer(ip, WEB_SERVER_PORT);
            server->on("/get-id", HTTPMethod::HTTP_POST, get_id_handler_v2);
            server->on("/start-branch", HTTPMethod::HTTP_POST, start_branch_handler);
            server->on("/branch-data", HTTPMethod::HTTP_POST, branch_data_handler);
            server->on("/end-branch", HTTPMethod::HTTP_POST, end_branch_handler);
            server->on("/", HTTPMethod::HTTP_GET, root_handler);
            server->begin();
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME launch(IPAddress ip)
        {
            // register_cb
            children = new std::map<String, ClientVitals>();
            server_init(ip);
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME request_get(WiFiClient &client, const String &url)
        {
            HTTPClient http;
            String payload = "-"; // if first char is - it means error in request

            if (http.begin(client, url.c_str()))
            {
                int http_code = http.GET();

                if (http_code > 0)
                {
                    PrintMessage("[HTTP] GET... code: " + String(http_code));

                    if (http_code == HTTP_CODE_OK)
                        payload = http.getString();
                    else
                        PrintMessage("[HTTP] GET... response: " + http.getString());
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
        FN_NAME request_post(WiFiClient &client, const String &url, const String &data)
        {
            HTTPClient http;
            String payload = "-"; // if first char is - it means error in request

            if (http.begin(client, url.c_str()))
            {
                int http_code = http.POST(data);

                if (http_code > 0)
                {
                    PrintMessage("[HTTP] POST... code: " + String(http_code));

                    if (http_code == HTTP_CODE_OK)
                        payload = http.getString();
                    else
                        PrintMessage("[HTTP] POST... response: " + http.getString());
                }
                else
                {
                    Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(http_code).c_str());
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
            YIELD
            String host = ip.toString();
            WiFiClient client;
            String url = "http://" + host + ":" + String(WEB_CLIENT_PORT) + "/get-id";
            PrintMessage(STR("[HTTP] Requesting URL: ") + url);
            return request_post(client, url, WiFi.macAddress());
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME start_branch(IPAddress ip)
        {
            YIELD
            String host = ip.toString();
            WiFiClient client;
            String url = "http://" + host + ":" + String(WEB_CLIENT_PORT) + "/start-branch";
            PrintMessage(STR("[HTTP] Requesting URL: ") + url);
            return request_get(client, url);
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME branch_data(IPAddress ip, const String &data)
        {
            YIELD
            String host = ip.toString();
            WiFiClient client;
            String url = "http://" + host + ":" + String(WEB_CLIENT_PORT) + "/branch-data";
            PrintMessage(STR("[HTTP] Requesting URL: ") + url);
            return request_post(client, url, data);
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME end_branch(IPAddress ip)
        {
            YIELD
            String host = ip.toString();
            WiFiClient client;
            String url = "http://" + host + ":" + String(WEB_CLIENT_PORT) + "/end-branch";
            PrintMessage(STR("[HTTP] Requesting URL: ") + url);
            return request_get(client, url);
        }
    }
}

#endif