#ifndef UDP_H
#define UDP_H

#include "../base.h"

#include <vector>
#include <WiFiUdp.h>

namespace Platform
{
    namespace UDP
    {
        DECLARE(WiFiUDP, *udp);
        DECLARE_INIT(uint, packet_size, 0);

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(void)
        FN_NAME launch()
        {
            udp = new WiFiUDP;
            udp->begin(UDP_PORT);
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(bool)
        FN_NAME packet_received()
        {
            packet_size = udp->parsePacket();
            return packet_size;
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME send_packet(const Base::Data::IPort &ipp, const String &packet, bool sync = false)
        {
            udp->beginPacket(ipp.ip, ipp.port);
            udp->write(packet.c_str());
            udp->endPacket();

            if (!sync)
                NON_VOID(String)

            for (int i = 0; !udp->available(); i++)
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
            DEBUG_PRINT("[UDP] Received packet! Size: ");
            DEBUG_PRINTLN(packet_size);

            String packet;
            packet.reserve(packet_size);

            udp->read(packet.begin(), packet_size + 1);

            DEBUG_PRINT("[UDP] Packet received: ");
            DEBUG_PRINTLN(packet);

            // send_packet({udp->remoteIP(), udp->remotePort()}, String("some data"));

            return packet;
        }
    }
}

#endif