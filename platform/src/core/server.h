#ifndef SERVER_H
#define SERVER_H

#include <ESP8266WebServer.h>

namespace Platform
{
    namespace Server
    {
        ESP8266WebServer *server;

        void launch(IPAddress ip)
        {
            server = new ESP8266WebServer(ip, 80);
        }
    }
}

#endif // SERVER_H