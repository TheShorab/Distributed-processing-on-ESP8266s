#ifndef HTTP_H
#define HTTP_H

#include "../base.h"

#include <vector>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

namespace Platform
{
    namespace HTTP
    {
        DECLARE(ESP8266WebServer, *server);
        DECLARE(std::vector<IPAddress>, *children);

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME get_id_handler()
        {
            size_t newID = children->size();
            children->push_back(server->client().localIP());
            server->send(200, "text/plain", String(newID));
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME launch(IPAddress ip)
        {
            // register_cb
            children = new std::vector<IPAddress>();
            server = new ESP8266WebServer(ip, WEB_SERVER_PORT);
            server->on("get-id", HTTPMethod::HTTP_GET, get_id_handler);
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME request(String url)
        {
            HTTPClient http;
            String payload = "-"; // if first char is - it means error in request

            if (http.begin(server->client(), url))
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
                Serial.printf("[HTTP] Unable to connect\n");
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