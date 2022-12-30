#ifndef CLOUD_LINKER_H
#define CLOUD_LINKER_H

#include "./base.h"
#include "./server/udp.h"
#include "./server/tcp.h"
#include "./server/esp-now.h"

#include <ESP8266WebServer.h>

namespace Platform
{
    namespace CloudLinker
    {
        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME read_function(const IPAddress &ip, uint16_t port, const String &packet)
        {
            read(ip, port, String("function:" + packet));
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME read_variable(const IPAddress &ip, uint16_t port, const String &packet)
        {
            read(ip, port, String("variable:" + packet));
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(String)
        FN_NAME read(const IPAddress &ip, uint16_t port, const String &packet)
        {
            switch (Platform::Base::Flags::network)
            {
            case USE_UDP:
                return Platform::UDP::send_packet({ip, port}, packet, true);

            case USE_TCP:
                return Platform::TCP::send_packet({ip, port}, packet, true);

            case USE_NOW:
                return Platform::NOW::send_packet({ip, port}, packet, true);
            }
        }
    }
}

#endif // CLOUD_LINKER
