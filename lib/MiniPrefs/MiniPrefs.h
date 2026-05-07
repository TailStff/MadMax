#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_FRAM_I2C.h>

// TestDeVariableDe32OctsDeLongueur
static constexpr uint8_t MAX_KEY_LENGTH = 32;     // à ajuster
static constexpr uint8_t MAX_INDEX_ENTRIES = 255; // à ajuster, nombre d'objet dont on trace l'adresse en FRAM et son hash de clé pour accélérer les recherches. Avec 255 entrées, on utilise 255*4=1020 bytes de RAM pour l'index, ce qui est déjà pas mal. Si on veut plus d'entrées, il faudrait faire une structure d'index plus compacte (ex: 1 byte de hash + 3 bytes d'adresse) ou faire un index hiérarchique.
static constexpr uint8_t MAX_DATA_BUFFER = 128;   // à ajuster

// Seuil de remplissage à partir duquel la défrag incrémentale démarre (0.0 - 1.0)
static constexpr float DEFRAG_THRESHOLD = 0.5f;

// Taille du buffer de batch pour la défrag.
// Avec des entrées de 32-128 bytes, 512 bytes regroupent ~4-16 entrées par write().
// Doit être >= sizeof(FramEntryHeader) + MAX_KEY_LENGTH + MAX_DATA_BUFFER.
static constexpr uint16_t DEFRAG_BUF_SIZE = 512;

#define FRAM_SIZE 32768            // taille FRAM en bytes
#define MINIPREFS_MAGIC 0x504D5246 // "MPRF"
#define MINIPREFS_VERSION 2

class MiniPrefs;

struct __attribute__((packed)) FramHeader
{
    uint32_t magic;
    uint16_t version;
    uint16_t writePointer;
    uint16_t crc;
};

struct __attribute__((packed)) FramEntryHeader
{
    uint8_t keyLength;   // longueur clé
    uint16_t dataLength; // longueur data
    uint16_t crc;        // CRC16 sur clé+data
    uint8_t flags;       // 0 = valide, 1 = supprimé
    uint8_t keyHash;     // hash de la clé pour accélérer les recherches
};

struct IndexEntry
{
    uint8_t keyHash;
    uint8_t dummy; // pour l'alignement
    uint16_t addr;
};

/// @brief
struct DefragBatchEntry
{
    uint8_t keyHash;
    uint16_t dst;
};

class MiniPrefs
{
private:
    Adafruit_FRAM_I2C &fram;
    uint16_t framSize;
    uint16_t writePointer;

    uint16_t crc16(const uint8_t *data, uint16_t length);
    uint8_t crc8(const uint8_t *data, uint16_t length);

    // État de la défrag incrémentale
    bool _defragRunning;
    uint16_t _defragReadPtr;
    uint16_t _defragWritePtr;
    uint16_t _defragSrcStart;
    uint16_t _defragEndPtr;
    uint16_t _defragBufLen;
    bool _defragCorrupted;

    IndexEntry index[MAX_INDEX_ENTRIES];
    uint8_t indexCount;

    // Pre-allocated buffers pour la défrag (évite la stack et le heap)
    uint8_t _defragBuffer[DEFRAG_BUF_SIZE];

    void printByteBuffer(const uint8_t *buffer, size_t length)
    {
        Serial.print("[");
        for (size_t i = 0; i < length; i++)
        {
            if (i > 0)
                Serial.print(", ");
            if (buffer[i] < 0x10)
                Serial.print("0"); // pour avoir toujours deux chiffres
            Serial.print(buffer[i], HEX);
        }
        Serial.println("]");
    }

    void buildIndex();
    bool remove(const char *key);
    bool removeAddress(const char *key, uint16_t addr);
    bool sanityCheck(FramEntryHeader &h, uint16_t addr);
    void defragment();
    void writeEntry(uint16_t addr, FramEntryHeader &h, const char *key, const uint8_t *data);
    void updateHeader();

public:
    MiniPrefs(Adafruit_FRAM_I2C &f, uint16_t size = FRAM_SIZE);

    bool isDefragRunning() const;
    void begin(); // scan initial writePointer
    void Reinit(FramHeader &header);
    bool Put(const char *key, const uint8_t *data, uint16_t length);
    bool Get(const char *key, uint8_t *buffer, uint16_t maxLength, uint16_t &addr, uint16_t &outLength);
    bool defragStep();
    float GetFRAMUsage();
};