#define SERIALDEBUG

#include "MiniPrefs.h"

/////////////////////////
// CRC16 Modbus
uint16_t MiniPrefs::crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

uint8_t MiniPrefs::crc8(const uint8_t *data, uint16_t length)
{
    uint8_t hash = 0;

    while (length--)
        hash = (hash * 33) ^ *data++;

    return hash;
}

/////////////////////////
// Constructeur
MiniPrefs::MiniPrefs(Adafruit_FRAM_I2C &f, uint16_t size)
    : fram(f),
      framSize(size),
      writePointer(0),
      indexCount(0),
      _defragRunning(false),
      _defragReadPtr(0),
      _defragWritePtr(0),
      _defragEndPtr(0),
      _defragSrcStart(0),
      _defragBufLen(0),
      _defragCorrupted(false)
{
#ifdef SERIALDEBUG
    Serial.println("MiniPrefs constructor called");
#endif
}

/////////////////////////
// Scan FRAM pour trouver writePointer + construire l'index RAM
void MiniPrefs::begin()
{
#ifdef SERIALDEBUG
    Serial.println("MiniPrefs::begin() called");
#endif

    FramHeader header;
    fram.read(0, (uint8_t *)&header, sizeof(header));

    if (header.magic != MINIPREFS_MAGIC || header.version != MINIPREFS_VERSION)
    {
#ifdef SERIALDEBUG
        Serial.println("MiniPrefs: FRAM not initialized -> reinit");
#endif
        Reinit(header);
        return;
    }

#ifdef SERIALDEBUG
    Serial.println("MiniPrefs: FRAM header found, validating CRC");
#endif

    uint16_t check = crc16((uint8_t *)&header, sizeof(header) - 2);
    if (check != header.crc)
    {
#ifdef SERIALDEBUG
        Serial.println("MiniPrefs: header CRC invalid -> reinit");
#endif
        Reinit(header);
        return;
    }

    writePointer = header.writePointer;

    // Scan unique au démarrage pour construire l'index RAM
    // Après ce scan, remove()/Get() n'ont plus besoin de parcourir la FRAM
    buildIndex();

#ifdef SERIALDEBUG
    Serial.printf("MiniPrefs ready, writePointer=%u, indexCount=%u\n", writePointer, indexCount);
#endif
}

void MiniPrefs::Reinit(FramHeader &header)
{
    header.magic = MINIPREFS_MAGIC;
    header.version = MINIPREFS_VERSION;
    header.writePointer = sizeof(FramHeader);
    header.crc = crc16((uint8_t *)&header, sizeof(header) - 2);

    fram.write(0, (uint8_t *)&header, sizeof(header));

    writePointer = header.writePointer;
    indexCount = 0;
    _defragRunning = false;
}

/////////////////////////
// Scan FRAM -> construction de l'index RAM
// Appelé une seule fois dans begin(), et à la fin de chaque défrag.
// @remark Le keyHash peut provoquer des collisions — c'est un compromis accepté.
//         Get() et remove() vérifient toujours la clé réelle après un match de hash.
void MiniPrefs::buildIndex()
{
    indexCount = 0;

    uint16_t addr = sizeof(FramHeader);

    while (addr < writePointer)
    {
        FramEntryHeader h;
        fram.read(addr, (uint8_t *)&h, sizeof(h));

        if (!sanityCheck(h, addr))
            break;

        if (h.flags == 0 && indexCount < MAX_INDEX_ENTRIES)
        {
            index[indexCount].keyHash = h.keyHash;
            index[indexCount].addr = addr;
            indexCount++;
        }

        addr += sizeof(h) + h.keyLength + h.dataLength;
    }
}

/////////////////////////
// Écriture d'une entrée complète en FRAM à une adresse donnée.
// Calcule le CRC sur key+data, assemble header+key+data en RAM,
// puis écrit en un seul appel I2C.
void MiniPrefs::writeEntry(uint16_t addr, FramEntryHeader &h, const char *key, const uint8_t *data)
{
    // CRC calculé sur key + data avant assemblage — une seule passe
    static uint8_t tmp[MAX_KEY_LENGTH + MAX_DATA_BUFFER];
    memcpy(tmp, key, h.keyLength);
    memcpy(tmp + h.keyLength, data, h.dataLength);
    h.crc = crc16(tmp, h.keyLength + h.dataLength);

    // Assemblage puis écriture en un seul appel I2C
    static uint8_t entryBuffer[sizeof(FramEntryHeader) + MAX_KEY_LENGTH + MAX_DATA_BUFFER];
    memcpy(entryBuffer, &h, sizeof(h));
    memcpy(entryBuffer + sizeof(h), key, h.keyLength);
    memcpy(entryBuffer + sizeof(h) + h.keyLength, data, h.dataLength);

    fram.write(addr, entryBuffer, sizeof(h) + h.keyLength + h.dataLength);
}

/////////////////////////
// Ajout / mise à jour
bool MiniPrefs::Put(const char *key, const uint8_t *data, uint16_t length)
{
#ifdef SERIALDEBUG
    Serial.printf("MiniPrefs::Put(key=%s, length=%u) called\n", key, length);
#endif

    uint8_t keyLen = strlen(key);
    uint8_t keyHash = crc8((uint8_t *)key, keyLen);
    uint16_t existingAddr = 0xFFFF;

    if (keyLen > MAX_KEY_LENGTH || length > MAX_DATA_BUFFER)
        return false;

    // Comparaison avant écriture : si la valeur est identique, on n'écrit rien
    {
        static uint8_t existing[MAX_DATA_BUFFER];
        uint16_t existingLength = 0;

        if (Get(key, existing, MAX_DATA_BUFFER, existingAddr, existingLength))
            if (existingLength == length && memcmp(existing, data, length) == 0)
            {
#ifdef SERIALDEBUG
                Serial.printf("Entry with key '%s' already exists with identical data, skipping FRAM write\n", key);
#endif
                return true; // Donnée identique, aucune écriture FRAM
            }
    }

    // Si la clé existe avec la même taille de données : update in-place.
    // Pas de remove(), pas de writePointer++, pas de mise à jour d'index.
    if (existingAddr != 0xFFFF)
    {
        FramEntryHeader h;
        fram.read(existingAddr, (uint8_t *)&h, sizeof(h));

        if (!sanityCheck(h, existingAddr))
            return false;

        if (h.dataLength == length)
        {
#ifdef SERIALDEBUG
            Serial.printf("Updating entry in place at addr %u\n", existingAddr);
#endif
            writeEntry(existingAddr, h, key, data);
            return true;
        }
    }

    // Taille différente : supprimer l'ancienne entrée et en écrire une nouvelle
    // remove(key);
    removeAddress(key, existingAddr);

    FramEntryHeader h;
    h.keyLength = keyLen;
    h.dataLength = length;
    h.flags = 0;
    h.keyHash = keyHash;

    uint16_t entrySize = sizeof(h) + h.keyLength + h.dataLength;

    // Si plus de place : défrag bloquante de secours.
    // En fonctionnement normal ce cas ne devrait pas arriver si defragStep()
    // est appelé régulièrement (seuil à DEFRAG_THRESHOLD).
    if (writePointer + entrySize > framSize)
    {
        Serial.println("MiniPrefs: DEFRAGMENTATION (emergency blocking)");
        defragment();
        if (writePointer + entrySize > framSize)
        {
            Serial.println("MiniPrefs: not enough space even after defragmentation");
            return false;
        }
    }

    if (indexCount >= MAX_INDEX_ENTRIES)
    {
        Serial.println("MiniPrefs: index full");
        return false;
    }

#ifdef SERIALDEBUG
    Serial.printf("Writing entry at addr %u: keyLength=%u, dataLength=%u\n", writePointer, h.keyLength, h.dataLength);
#endif

    writeEntry(writePointer, h, key, data);

    index[indexCount].keyHash = h.keyHash;
    index[indexCount].addr = writePointer;
    indexCount++;

    writePointer += entrySize;

    updateHeader();

    return true;
}

/////////////////////////
// Lecture
bool MiniPrefs::Get(const char *key, uint8_t *buffer, uint16_t maxLength, uint16_t &addr, uint16_t &outLength)
{
#ifdef SERIALDEBUG
    Serial.printf("MiniPrefs::Get(key=%s, maxLength=%u) called\n", key, maxLength);
#endif

    uint8_t keyLen = strlen(key);
    uint8_t keyHash = crc8((uint8_t *)key, keyLen);

    static uint8_t tmp[MAX_KEY_LENGTH + MAX_DATA_BUFFER];

    for (uint8_t i = 0; i < indexCount; i++)
    {
        if (index[i].keyHash != keyHash)
            continue;

        // Hash identique — vérification de la clé réelle en FRAM (anti-collision)
        FramEntryHeader h;
        fram.read(index[i].addr, (uint8_t *)&h, sizeof(h));

        if (!sanityCheck(h, index[i].addr))
            continue;

        char storedKey[MAX_KEY_LENGTH + 1];
        fram.read(index[i].addr + sizeof(h), (uint8_t *)storedKey, h.keyLength);
        storedKey[h.keyLength] = 0;

        if (strcmp(storedKey, key) != 0)
            continue; // Collision hash, mauvaise entrée

        outLength = min(h.dataLength, maxLength);
        fram.read(index[i].addr + sizeof(h) + h.keyLength, buffer, outLength);

        // Vérification CRC
        memcpy(tmp, storedKey, h.keyLength);
        memcpy(tmp + h.keyLength, buffer, outLength);

        uint16_t check = crc16(tmp, h.keyLength + h.dataLength);
        if (check != h.crc)
        {
            Serial.println("MiniPrefs: data CRC invalid");
            return false;
        }

        addr = index[i].addr;
        return true;
    }

    return false;
}

/// @brief Removes a preference entry by its key, working version but not optimized (scans the FRAM to find the entry, marks it as deleted, and updates the index).
/// @param key The key of the entry to remove
/// @return True if the entry was found and marked as deleted, false otherwise
bool MiniPrefs::remove(const char *key)
{
#ifdef SERIALDEBUG
    Serial.printf("MiniPrefs::remove(key=%s) called\n", key);
#endif

    uint8_t keyLen = strlen(key);
    uint8_t keyHash = crc8((uint8_t *)key, keyLen);

    for (uint8_t i = 0; i < indexCount; i++)
    {
        if (index[i].keyHash != keyHash)
            continue;

        FramEntryHeader h;
        fram.read(index[i].addr, (uint8_t *)&h, sizeof(h));

        char storedKey[MAX_KEY_LENGTH + 1];
        fram.read(index[i].addr + sizeof(h), (uint8_t *)storedKey, h.keyLength);
        storedKey[h.keyLength] = 0;

        if (strcmp(storedKey, key) != 0)
            continue;

        h.flags = 1;
        fram.write(index[i].addr, (uint8_t *)&h, sizeof(h));

        // Swap avec le dernier pour éviter un décalage dans l'index
        index[i] = index[--indexCount];
        return true;
    }

    return false;
}

/// @brief Removes a preference entry by its address, optimized version that directly marks the entry as deleted in FRAM and updates the index without scanning for the key (used internally when the address is already known).
/// @param key The key of the entry to remove
/// @param addr The address of the entry to remove
/// @return True if the entry was found and marked as deleted, false otherwise
bool MiniPrefs::removeAddress(const char *key, uint16_t addr)
{
#ifdef SERIALDEBUG
    Serial.printf("MiniPrefs::remove(key=%s, addr=%u) called\n", key, addr);
#endif

    if (addr == 0xFFFF)
    {
#ifdef SERIALDEBUG
        Serial.println("MiniPrefs::removeAddress: invalid address 0xFFFF");
#endif
        return false;
    }

    FramEntryHeader h;
    fram.read(addr, (uint8_t *)&h, sizeof(h));

    // Swap avec le dernier pour éviter un décalage dans l'index
    for (uint8_t i = 0; i < indexCount; i++)
    {
        if (index[i].addr == addr)
        {
            h.flags = 1;
            fram.write(addr, (uint8_t *)&h, sizeof(h));
            index[i] = index[--indexCount];
            return true;
        }
    }
    return false;
}

bool MiniPrefs::sanityCheck(FramEntryHeader &h, uint16_t addr)
{
    if (h.keyLength == (byte)0xFF || h.dataLength == 0xFFFF)
        return false;

    if (h.keyLength == 0 || h.keyLength > MAX_KEY_LENGTH)
        return false;

    if (h.dataLength > MAX_DATA_BUFFER)
        return false;

    if (addr + sizeof(h) + h.keyLength + h.dataLength > framSize)
        return false;

    return true;
}

/////////////////////////
// Défragmentation bloquante
//
// Copie par chunks de DEFRAG_CHUNK_SIZE bytes.
// Invariant anti-chevauchement : writePtr <= readPtr toujours.
// Si l'écart src/dst < chunk, on tronque le chunk à cet écart.
void MiniPrefs::defragment()
{
    static const uint16_t DEFRAG_CHUNK_SIZE = 96;
    static uint8_t buffer[DEFRAG_CHUNK_SIZE];

    uint16_t readPtr = sizeof(FramHeader);
    uint16_t writePtr = sizeof(FramHeader);

    indexCount = 0;

    while (readPtr < writePointer)
    {
        FramEntryHeader h;
        fram.read(readPtr, (uint8_t *)&h, sizeof(h));

        if (!sanityCheck(h, readPtr))
            break;

        uint16_t entrySize = sizeof(h) + h.keyLength + h.dataLength;

        if (h.flags == 0)
        {
            if (readPtr != writePtr)
            {
                uint16_t remaining = entrySize;
                uint16_t src = readPtr;
                uint16_t dst = writePtr;

                while (remaining > 0)
                {
                    uint16_t gap = src - dst;
                    uint16_t chunk = remaining > DEFRAG_CHUNK_SIZE ? DEFRAG_CHUNK_SIZE : remaining;
                    if (chunk > gap)
                        chunk = gap;

                    fram.read(src, buffer, chunk);
                    fram.write(dst, buffer, chunk);

                    src += chunk;
                    dst += chunk;
                    remaining -= chunk;
                }
            }

            if (indexCount < MAX_INDEX_ENTRIES)
            {
                index[indexCount].keyHash = h.keyHash;
                index[indexCount].addr = writePtr;
                indexCount++;
            }

            writePtr += entrySize;
        }

        readPtr += entrySize;
    }

    writePointer = writePtr;
    updateHeader();
}

/////////////////////////
// Défragmentation incrémentale — à appeler à chaque iteration de loop()
//
// Démarre automatiquement quand le taux de remplissage dépasse DEFRAG_THRESHOLD.
// Traite un batch par appel (DEFRAG_BUF_SIZE bytes max) puis rend la main.
//
// Au démarrage, writePointer est capturé dans _defragEndPtr.
// Les Put() pendant la défrag écrivent après cette limite — ils ne sont
// jamais touchés par les steps.
//
// L'index RAM n'est pas modifié pendant les steps — Get()/Put()/remove()
// continuent à fonctionner avec les anciennes adresses, valides car on
// compacte vers le bas sans jamais écraser une zone non encore lue.
//
// À la fin, les éventuelles nouvelles entrées (entre _defragEndPtr et
// writePointer) sont déplacées juste après _defragWritePtr pour combler
// le trou, puis BuildIndex() reconstruit l'index proprement.
//
// Retourne true  : défrag terminée ou pas nécessaire
//          false : encore du travail, rappeler au prochain tick
bool MiniPrefs::defragStep()
{
    // --- Démarrage automatique si seuil dépassé ---
    if (!_defragRunning)
    {
        float usage = (float)writePointer / (float)framSize;
        if (usage < DEFRAG_THRESHOLD)
            return true;

        _defragEndPtr = writePointer;
        _defragReadPtr = sizeof(FramHeader);
        _defragWritePtr = sizeof(FramHeader);
        _defragCorrupted = false;
        _defragRunning = true;

#ifdef SERIALDEBUG
        Serial.printf("MiniPrefs: incremental defrag started (usage=%.1f%%, endPtr=%u)\n",
                      usage * 100.0f, _defragEndPtr);
#endif
    }

    // --- Fin de défrag ---
    if (_defragReadPtr >= _defragEndPtr || _defragCorrupted)
    {
        // Déplacer les entrées ajoutées pendant la défrag (entre _defragEndPtr
        // et writePointer) juste après _defragWritePtr pour combler le trou.
        // Sans ce déplacement, writePointer serait incorrect et la FRAM aurait
        // un trou entre _defragWritePtr et _defragEndPtr.
        uint16_t appended = writePointer - _defragEndPtr;

        if (appended > 0)
        {
            // _defragWritePtr <= _defragEndPtr toujours — pas de chevauchement
            uint16_t src = _defragEndPtr;
            uint16_t dst = _defragWritePtr;
            uint16_t remaining = appended;

            while (remaining > 0)
            {
                uint16_t chunk = remaining > DEFRAG_BUF_SIZE ? DEFRAG_BUF_SIZE : remaining;
                fram.read(src, _defragBuffer, chunk);
                fram.write(dst, _defragBuffer, chunk);
                src += chunk;
                dst += chunk;
                remaining -= chunk;
            }
        }

        writePointer = _defragWritePtr + appended;
        _defragRunning = false;
        updateHeader();
        buildIndex();

#ifdef SERIALDEBUG
        Serial.printf("MiniPrefs: incremental defrag done, writePointer=%u, indexCount=%u\n",
                      writePointer, indexCount);
#endif
        return true;
    }

    // --- Phase 1 : accumuler des entrées valides dans le buffer ---
    _defragBufLen = 0;
    _defragSrcStart = 0xFFFF; // sentinelle "pas encore trouvé"

    uint16_t scanPtr = _defragReadPtr;

    while (scanPtr < _defragEndPtr)
    {
        FramEntryHeader h;
        fram.read(scanPtr, (uint8_t *)&h, sizeof(h));

        if (!sanityCheck(h, scanPtr))
        {
            _defragCorrupted = true;
            break;
        }

        uint16_t entrySize = sizeof(h) + h.keyLength + h.dataLength;

        if (h.flags == 0)
        {
            if (_defragBufLen + entrySize > DEFRAG_BUF_SIZE)
                break; // Buffer plein — flush puis prochain step

            if (_defragSrcStart == 0xFFFF)
                _defragSrcStart = scanPtr; // première entrée valide du batch

            fram.read(scanPtr, _defragBuffer + _defragBufLen, entrySize);
            _defragBufLen += entrySize;
        }

        scanPtr += entrySize;
    }

    _defragReadPtr = scanPtr;

    // Rien d'utile dans ce batch (que des entrées supprimées)
    if (_defragBufLen == 0 || _defragSrcStart == 0xFFFF)
        return false;

    // --- Phase 2 : écrire le batch en FRAM ---
    // gap = _defragSrcStart - _defragWritePtr : écart réel entre src et dst.
    // Garanti >= 0 car on compacte vers le bas.
    // Si bufLen <= gap : un seul write(), pas de chevauchement possible.
    // Sinon : écriture par tranches de gap.
    uint16_t gap = _defragSrcStart - _defragWritePtr;

    if (gap == 0)
    {
        // Déjà en place, rien à écrire
    }
    else if (_defragBufLen <= gap)
    {
        fram.write(_defragWritePtr, _defragBuffer, _defragBufLen);
    }
    else
    {
        uint16_t remaining = _defragBufLen;
        uint16_t bufOff = 0;
        uint16_t dst = _defragWritePtr;

        while (remaining > 0)
        {
            uint16_t chunk = remaining > gap ? gap : remaining;
            fram.write(dst, _defragBuffer + bufOff, chunk);
            bufOff += chunk;
            dst += chunk;
            remaining -= chunk;
        }
    }

    _defragWritePtr += _defragBufLen;

    return false; // Encore du travail
}

void MiniPrefs::updateHeader()
{
    FramHeader header;

    header.magic = MINIPREFS_MAGIC;
    header.version = MINIPREFS_VERSION;
    header.writePointer = writePointer;
    header.crc = crc16((uint8_t *)&header, sizeof(header) - 2);

    fram.write(0, (uint8_t *)&header, sizeof(header));
}

float MiniPrefs::GetFRAMUsage()
{
    return (float)writePointer / (float)framSize * 100.0f;
}

bool MiniPrefs::isDefragRunning() const
{
    return _defragRunning;
}