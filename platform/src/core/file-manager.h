#include "base.h"

#include "SD.h"
#include "SPI.h"
#include "SdCard/SdCardInfo.h"
#include <string>

#define SD_MODE 0
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
                Serial.println("Failed to open directory");
                return;
            }
            if (!root.isDirectory())
            {
                Serial.println("Not a directory");
                return;
            }

            File file = root.openNextFile();
            while (file)
            {
                if (file.isDirectory())
                {
                    Serial.print("  DIR : ");
                    Serial.println(file.name());
                    if (levels)
                    {
                        listDir(file.name(), levels - 1);
                    }
                }
                else
                {
                    Serial.print("  FILE: ");
                    Serial.print(file.name());
                    Serial.print("  SIZE: ");
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
                Serial.println("Dir created");
            }
            else
            {
                Serial.println("mkdir failed");
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME removeDir(const char *path)
        {
            Serial.printf("Removing Dir: %s\n", path);
            if (SD.rmdir(path))
            {
                Serial.println("Dir removed");
            }
            else
            {
                Serial.println("rmdir failed");
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
        FN_NAME readFile(const char *path)
        {
            Serial.printf("Reading file: %s\n", path);

            File file = SD.open(path, "r+");
            if (!file)
            {
                Serial.println("Failed to open file for reading");
                return;
            }

            Serial.print("Read from file: ");
            while (file.available())
            {
                // Serial.write();
                Println(readLine(file).c_str());
            }

            file.close();
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(bool)
        FN_NAME fileExists(const char *path)
        {
            Serial.printf("Finding file: %s\n", path);

            File file = SD.open(path, "r+");
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
        FN_RETURN_TYPE(void)
        FN_NAME writeFile(const char *path, const char *message)
        {
            Serial.printf("Writing file: %s\n", path);

            File file = SD.open(path, "w+");
            if (!file)
            {
                Serial.println("Failed to open file for writing");
                return;
            }
            if (file.print(message))
            {
                Serial.println("File written");
            }
            else
            {
                Serial.println("Write failed");
            }
            file.close();
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME appendFile(const char *path, const char *message)
        {
            Serial.printf("Appending to file: %s\n", path);

            File file = SD.open(path, "a");
            if (!file)
            {
                Serial.println("Failed to open file for appending");
                return;
            }
            if (file.print(message))
            {
                Serial.println("Message appended");
            }
            else
            {
                Serial.println("Append failed");
            }
            file.close();
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME renameFile(const char *path1, const char *path2)
        {
            Serial.printf("Renaming file %s to %s\n", path1, path2);
            if (SD.rename(path1, path2))
            {
                Serial.println("File renamed");
            }
            else
            {
                Serial.println("Rename failed");
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(void)
        FN_NAME deleteFile(const char *path)
        {
            Serial.printf("Deleting file: %s\n", path);
            if (SD.remove(path))
            {
                Serial.println("File deleted");
            }
            else
            {
                Serial.println("Delete failed");
            }
        }

        FN_ATTRIBUTES(INDEPENDENT ICACHE_FLASH_ATTR)
        FN_RETURN_TYPE(bool)
        FN_NAME initialize()
        {
            SPIClass spi;
            spi.pins(SCK, MISO, MOSI, CS);
            spi.begin();

            if (!SD.begin(CS, /* spi */ 8000000U))
            {
                Serial.println("Card Mount Failed");
                return false;
            }

            uint8_t cardType = SD.type();
            Serial.print("SD Card Type: ");
            if (cardType == sdfat::SD_CARD_TYPE_SD1)
            {
                Serial.println("MMC");
            }
            else if (cardType == sdfat::SD_CARD_TYPE_SD2)
            {
                Serial.println("SDSC");
            }
            else if (cardType == sdfat::SD_CARD_TYPE_SDHC)
            {
                Serial.println("SDHC");
            }
            else
            {
                Serial.println("UNKNOWN");
            }

            uint64_t cardSize = SD.size() / (1024 * 1024 * 1024);
            Serial.printf("SD Card Size: %lluGB\n", cardSize);
        }
    }
}