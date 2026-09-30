/**
 * @file eezml_uart_persist.c
 * @brief Persist the last pushed hot-reload document in the "storage" flash
 *        partition, so a reboot keeps the pushed UI instead of falling back
 *        to the compiled-in (EMBED) document.
 *
 * On-disk record (single slot, little-endian):
 *   magic "EEZD" | u16 version | u16 name_len | u32 xml_len | u32 xml_crc32
 *   | name bytes | xml bytes
 *
 * Corruption (bad magic / bad crc / silly sizes) simply reports "nothing
 * persisted" — the caller falls back to the embedded document.
 *
 * Partition: the data/spiffs "storage" entry from partitions.csv, written
 * through the esp_partition API without mounting any filesystem.
 */

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_partition.h"
#include "eezml_uart_persist.h"

#define EEZML_PERSIST_MAGIC       0x445A4545U  /* "EEZD" little-endian */
#define EEZML_PERSIST_VERSION     1
#define EEZML_PERSIST_MAX_XML     (16 * 1024)  /* matches transport buffer */
#define EEZML_PERSIST_MAX_NAME    32

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint16_t version;
    uint16_t name_len;
    uint32_t xml_len;
    uint32_t xml_crc32;
} persist_hdr_t;

static const char * TAG = "eezml_persist";
static const esp_partition_t * s_part;

static const esp_partition_t * part(void)
{
    if (!s_part) {
        s_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                          ESP_PARTITION_SUBTYPE_DATA_SPIFFS,
                                          "storage");
        if (!s_part) {
            ESP_LOGW(TAG, "no \"storage\" partition found, persistence disabled");
        }
    }
    return s_part;
}

/* same crc32 as the wire protocol */
static uint32_t crc32_buf(const uint8_t * data, size_t len)
{
    uint32_t c = ~0U;
    for (size_t i = 0; i < len; i++) {
        c ^= data[i];
        for (int k = 0; k < 8; k++)
            c = (c >> 1) ^ (0xEDB88320U & (-(int32_t)(c & 1)));
    }
    return ~c;
}

esp_err_t eezml_uart_persist_store(const char * name, const char * xml, size_t len)
{
    const esp_partition_t * p = part();
    if (!p) return ESP_ERR_NOT_FOUND;
    if (!name || !xml || len == 0 || len > EEZML_PERSIST_MAX_XML) return ESP_ERR_INVALID_ARG;

    size_t name_len = strlen(name);
    if (name_len == 0 || name_len > EEZML_PERSIST_MAX_NAME) return ESP_ERR_INVALID_ARG;

    persist_hdr_t hdr = {
        .magic = EEZML_PERSIST_MAGIC,
        .version = EEZML_PERSIST_VERSION,
        .name_len = (uint16_t)name_len,
        .xml_len = (uint32_t)len,
        .xml_crc32 = crc32_buf((const uint8_t *)xml, len),
    };

    size_t total = sizeof(hdr) + name_len + len;
    size_t erase_sz = (total + p->erase_size - 1) / p->erase_size * p->erase_size;
    if (erase_sz > p->size) return ESP_ERR_INVALID_SIZE;

    ESP_LOGI(TAG, "persisting \"%s\" (%u bytes) to @0x%lx",
             name, (unsigned)len, (unsigned long)p->address);

    esp_err_t err = esp_partition_erase_range(p, 0, erase_sz);
    if (err != ESP_OK) return err;

    /* header + name + xml in one contiguous image: build it in RAM
       (max ~16K + 32 + 12 — fine on the P4) */
    static uint8_t img[EEZML_PERSIST_MAX_XML + EEZML_PERSIST_MAX_NAME + sizeof(persist_hdr_t)];
    memcpy(img, &hdr, sizeof(hdr));
    memcpy(img + sizeof(hdr), name, name_len);
    memcpy(img + sizeof(hdr) + name_len, xml, len);

    err = esp_partition_write(p, 0, img, total);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "persisted ok (%u bytes flashed)", (unsigned)total);
    }
    return err;
}

/* Returns a malloc'd NUL-terminated copy of the persisted xml (caller
   frees), or NULL when nothing valid is stored. */
char * eezml_uart_persist_load(char * name_out, size_t name_sz)
{
    const esp_partition_t * p = part();
    if (!p) return NULL;

    persist_hdr_t hdr;
    if (esp_partition_read(p, 0, &hdr, sizeof(hdr)) != ESP_OK) return NULL;
    if (hdr.magic != EEZML_PERSIST_MAGIC) return NULL;
    if (hdr.version != EEZML_PERSIST_VERSION) return NULL;
    if (hdr.name_len == 0 || hdr.name_len > EEZML_PERSIST_MAX_NAME) return NULL;
    if (hdr.xml_len == 0 || hdr.xml_len > EEZML_PERSIST_MAX_XML) return NULL;

    char name[EEZML_PERSIST_MAX_NAME + 1];
    if (esp_partition_read(p, sizeof(hdr), name, hdr.name_len) != ESP_OK) return NULL;
    name[hdr.name_len] = '\0';

    char * xml = malloc(hdr.xml_len + 1);
    if (!xml) return NULL;
    if (esp_partition_read(p, sizeof(hdr) + hdr.name_len, xml, hdr.xml_len) != ESP_OK) {
        free(xml);
        return NULL;
    }
    xml[hdr.xml_len] = '\0';

    if (crc32_buf((const uint8_t *)xml, hdr.xml_len) != hdr.xml_crc32) {
        ESP_LOGW(TAG, "persisted record \"%s\" failed crc — ignored", name);
        free(xml);
        return NULL;
    }

    if (name_out && name_sz) {
        strncpy(name_out, name, name_sz - 1);
        name_out[name_sz - 1] = '\0';
    }
    ESP_LOGI(TAG, "persisted document found: \"%s\" (%u bytes)",
             name, (unsigned)hdr.xml_len);
    return xml;
}

esp_err_t eezml_uart_persist_wipe(void)
{
    const esp_partition_t * p = part();
    if (!p) return ESP_ERR_NOT_FOUND;
    esp_err_t err = esp_partition_erase_range(p, 0, p->erase_size);
    if (err == ESP_OK) ESP_LOGI(TAG, "persisted document wiped");
    return err;
}
