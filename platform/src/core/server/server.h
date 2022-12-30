#ifndef SERVER_H
#define SERVER_H

#include "../base.h"

#include "http.h"

namespace Platform
{
    namespace Server
    {
        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(void)
        FN_NAME launch(const IPAddress &ip)
        {
            Platform::HTTP::launch(ip);
        }
    }
}

#endif // SERVER_H