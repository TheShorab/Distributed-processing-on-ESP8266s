#ifndef FLASH_LIGHT_H
#define FLASH_LIGHT_H

#include "./base.h"

namespace Platform
{
    namespace FlashLight
    {
        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME initialize()
        {
            pinMode(LED_BUILTIN, OUTPUT);
            digitalWrite(LED_BUILTIN, HIGH);
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(void)
        FN_NAME dance(int8_t times)
        {
            while (times--)
            {
                digitalWrite(LED_BUILTIN, LOW);  // Turn the LED on by making the voltage LOW
                delay(250);                      // Wait for a second
                digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
                delay(250);                      // Wait for two seconds
            }
        }
    }
}

#endif // FLASH_LIGHT_H