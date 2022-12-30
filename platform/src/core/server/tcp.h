#ifndef TCP_H
#define TCP_H

#include "../base.h"

#include <ESP8266WiFi.h>
#include <vector>

namespace Platform
{
    namespace TCP
    {
        DECLARE(WiFiServer, *tcp);
        DECLARE_INIT(uint, packet_size, 0);

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(void)
        FN_NAME launch()
        {
            tcp = new WiFiServer(TCP_PORT);
            tcp->begin();
            tcp->setNoDelay(true);
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(bool)
        FN_NAME packet_received()
        {
            return tcp->hasClient();
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME send_packet(const Base::Data::IPort &ipp, const String &packet, bool sync = false)
        {
            WiFiClient cl;
            cl.connect(ipp.ip, ipp.port);

            for (int i = 0; !cl.connected() || !cl.availableForWrite(); i++)
            {
                if (i == 5)
                {
                    Base::terminate_execution(
                        PRETTY_ERROR("[TCP] After 6 tries, we cant connect to server: " + ipp.ip.toString(),
                                     send_packet));
                }
                delay(500);
            }

            cl.write(packet.c_str());

            if (!sync)
                NON_VOID(String)

            for (int i = 0; !tcp->hasClient(); i++)
            {
                if (i == 99)
                    Base::terminate_execution(
                        PRETTY_ERROR("[UDP] After 1 second, we cant get data from server: " + ipp.ip.toString(),
                                     send_packet));

                delay(10);
            }

            return handle_packet();
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME handle_packet()
        {
            WiFiClient client;

            client = tcp->available();

            DEBUG_PRINT("[TCP] New client");

            packet_size = client.available();

            if (packet_size)
            {
                String packet;
                packet.reserve(packet_size);
                client.read(packet.begin(), packet_size);

                DEBUG_PRINT("[TCP] Packet received: ");
                DEBUG_PRINTLN(packet);

                // client.write(String("some data").c_str());
            }
        }
    }
}

#endif