#include "base.h"

#include "SD.h"
#include "SPI.h"
#include "SdCard/SdCardInfo.h"
#include <string>

#define SD_MODE 2
#if SD_MODE == 0
#define __SD__ SD
#elif SD_MODE == 1
sdfat::SdFat sd;
#define __SD__ sd
#elif SD_MODE == 2
sdfat::SdFat32 sd;
#define __SD__ sd
#elif SD_MODE == 3
sdfat::SdExFat sd;
#define __SD__ sd
#endif

#define SCK D5
#define MISO D6
#define MOSI D7
#define CS D8

namespace Platform
{
    namespace FileManager
    {
        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME listDir(const String &dirname, uint8_t levels)
        {
            Serial.printf("Listing directory: %s\n", dirname);

            File root = SD.open(dirname, "w+");

            if (!root)
            {
                Serial.println(STR("Failed to open directory"));
                return;
            }
            if (!root.isDirectory())
            {
                Serial.println(STR("Not a directory"));
                return;
            }

            File file = root.openNextFile();
            while (file)
            {
                if (file.isDirectory())
                {
                    Serial.print(STR("  DIR : "));
                    Serial.println(file.name());
                    if (levels)
                    {
                        listDir(file.name(), levels - 1);
                    }
                }
                else
                {
                    Serial.print(STR("  FILE: "));
                    Serial.print(file.name());
                    Serial.print(STR("  SIZE: "));
                    Serial.println(file.size());
                }
                file = root.openNextFile();
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME createDir(const char *path)
        {
            Serial.printf("Creating Dir: %s\n", path);
            if (SD.mkdir(path))
            {
                Serial.println(STR("Dir created"));
            }
            else
            {
                Serial.println(STR("mkdir failed"));
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME removeDir(const char *path)
        {
            Serial.printf("Removing Dir: %s\n", path);
            if (SD.rmdir(path))
            {
                Serial.println(STR("Dir removed"));
            }
            else
            {
                Serial.println(STR("rmdir failed"));
            }
        }

        FN_ATTRIBUTES(INDEPENDENT)
        FN_RETURN_TYPE(std::string)
        FN_NAME readLine(File &file)
        {
            std::string s;
            for (char ch = (char)file.read(); file.available() && ch != '\n'; ch = (char)file.read())
                s.push_back(ch);

            return s;
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME readFile(File file, const char *path)
        {
            Serial.printf("Reading file: %s\n", path);

            if (!file)
            {
                Serial.println(STR("Failed to open file for reading"));
                return;
            }

            Serial.print(STR("Read from file: "));
            while (file.available())
            {
                Println(readLine(file).c_str());
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME readFile(const char *path)
        {
            File file = SD.open(path, "r+");
            readFile(file, path);
            file.close();
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(bool)
        FN_NAME fileExists(File file, const char *path)
        {
            Serial.printf("Finding file: %s\n", path);

            if (!file)
            {
                Serial.printf("File founded: %s\n", path);
                return true;
            }
            else
            {
                Serial.printf("File not founded: %s\n", path);
                return false;
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(bool)
        FN_NAME fileExists(const char *path)
        {
            fileExists(SD.open(path, "r+"), path);
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME writeFile(File file, const char *path, const char *message)
        {
            Serial.printf("Writing file: %s\n", path);

            if (!file)
            {
                Serial.println(STR("Failed to open file for writing"));
                return;
            }
            if (file.print(message))
            {
                Serial.println(STR("File written"));
            }
            else
            {
                Serial.println(STR("Write failed"));
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME writeFile(const char *path, const char *message)
        {
            File file = SD.open(path, "w+");
            writeFile(file, path, message);
            file.close();
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME appendFile(File file, const char *path, const char *message)
        {
            Serial.printf("Appending to file: %s\n", path);

            if (!file)
            {
                Serial.println(STR("Failed to open file for appending"));
                return;
            }
            if (file.print(message))
            {
                Serial.println(STR("Message appended"));
            }
            else
            {
                Serial.println(STR("Append failed"));
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME appendFile(const char *path, const char *message)
        {
            File file = SD.open(path, "a");
            appendFile(file, path, message);
            file.close();
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME renameFile(const char *path1, const char *path2)
        {
            Serial.printf("Renaming file %s to %s\n", path1, path2);
            if (SD.rename(path1, path2))
            {
                Serial.println(STR("File renamed"));
            }
            else
            {
                Serial.println(STR("Rename failed"));
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME deleteFile(const char *path)
        {
            Serial.printf("Deleting file: %s\n", path);
            if (SD.remove(path))
            {
                Serial.println(STR("File deleted"));
            }
            else
            {
                Serial.println(STR("Delete failed"));
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(bool)
        FN_NAME initialize()
        {
            // DEBUG_PRINTLN(STR("invoked"));
            SPIClass spi;
            spi.pins(SCK, MISO, MOSI, CS);
            spi.begin();

            if (!SD.begin(CS, /* spi */ 8000000U))
            {
                Serial.println(STR("Card Mount Failed."));
                return false;
            }

            uint8_t cardType = SD.type();
            Serial.print(STR("SD Card Type: "));
            if (cardType == sdfat::SD_CARD_TYPE_SD1)
            {
                Serial.println(STR("MMC"));
            }
            else if (cardType == sdfat::SD_CARD_TYPE_SD2)
            {
                Serial.println(STR("SDSC"));
            }
            else if (cardType == sdfat::SD_CARD_TYPE_SDHC)
            {
                Serial.println(STR("SDHC"));
            }
            else
            {
                Serial.println(STR("UNKNOWN"));
            }

            uint64_t cardSize = SD.size() / (1024 * 1024 * 1024);
            Serial.printf("SD Card Size: %lluGB\n", cardSize);
        }
    }
}