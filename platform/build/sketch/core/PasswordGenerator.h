#line 1 "f:\\UT\\Term 1\\Computer Achitecture\\Project\\Distributed-processing-on-ESP8266s\\platform\\src\\core\\PasswordGenerator.h"
#include <bitset>

#define BITS 8

String numberToBinary(uint8_t number)
{
    std::bitset<BITS> bin(number);
    return String(bin.to_string().c_str());
}

uint8_t onesCompliment(uint8_t number)
{
    std::bitset<BITS> max;
    max.set();
    return number ^ (uint8_t)(max.to_ulong());
}

uint8_t towsCompliment(uint8_t number)
{
    return onesCompliment(number) + 1;
}

uint8_t binaryToNumber(String binary)
{
    std::bitset<BITS> bin(binary.c_str());
    return bin.to_ulong();
}

String section(uint8_t i, uint8_t number)
{
    if (i == 0)
        return numberToBinary(number);
    if (i == 1)
        return numberToBinary(onesCompliment(number) ^ number);
    if (i == 2)
        return numberToBinary(onesCompliment(number));
    if (i == 3)
        return numberToBinary(towsCompliment(number) ^ number);
    if (i == 4)
        return numberToBinary(towsCompliment(number));
}

String generatePassword(int8_t number)
{
    char *chars = "8.!2@#$%3^0&*(5)/-+=9><?1~`X64Z7"; // 32 ~ 5 bits
    String bits;
    String password;

    for (uint8_t i = 0; i < 5; i++)
    {
        bits += section(i, number);
        while (bits.length() >= 5)
        {
            password += chars[binaryToNumber(bits.substring(0, 5))];
            bits.remove(0, 5);
        }
    }

    return password;
}