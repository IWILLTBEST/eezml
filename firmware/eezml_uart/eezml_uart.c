/**
 * @file eezml_uart.c
 * @brief ESP-IDF port: hot-reload transport over the console UART.
 *
 * Wire protocol (line-framed):
 *   EEZML BEGIN <name> <size>\n
 *   <raw XML bytes x size>
 *   EEZML END <crc32-hex>\n
 *
 * A complete, CRC-verified packet is applied from an LVGL timer (i.e. on the
 * LVGL task, where it is safe to touch objects) via eezml_reload().
 * Console traffic and this protocol share UART0: keep `idf.py monitor`
 * closed while pushing.
 */

#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "eezml.h"

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#include "driver/usb_serial_jtag.h"
#else
#include "driver/uart.h"
#endif

#define EEZML_UART_NUM       UART_NUM_0
#define EEZML_UART_BAUD      115200
#define EEZML_BUF_MAX        (16 * 1024)
#define EEZML_RX_RING        2048
#define EEZML_APPLY_PERIOD   100   /* ms, LVGL timer period */
#define EEZML_STR2(x) #x
#define EEZML_STR(x)  EEZML_STR2(x)

static const char * TAG = "eezml_uart";

/* forward decls: definition order should not matter in this file */
static void feed_byte(uint8_t b);

static char     s_buf[EEZML_BUF_MAX];
static size_t   s_buf_len;
static size_t   s_expected;
static uint32_t s_crc;
static volatile bool s_pending;          /* packet ready to apply */
static char     s_line[96];
static size_t   s_line_len;

/* simple crc32 */
static uint32_t crc32_buf(const char * data, size_t len)
{
    uint32_t c = ~0U;
    for (size_t i = 0; i < len; i++) {
        c ^= (uint8_t)data[i];
        for (int k = 0; k < 8; k++)
            c = (c >> 1) ^ (0xEDB88320U & (-(int32_t)(c & 1)));
    }
    return ~c;
}

static int transport_read(uint8_t * buf, int buf_sz)
{
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    return usb_serial_jtag_read_bytes(buf, buf_sz, pdMS_TO_TICKS(200));
#else
    return uart_read_bytes(EEZML_UART_NUM, buf, buf_sz, pdMS_TO_TICKS(200));
#endif
}

static int transport_write(const char * data, size_t len)
{
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    return usb_serial_jtag_write_bytes((const uint8_t *)data, len, pdMS_TO_TICKS(100));
#else
    return uart_write_bytes(EEZML_UART_NUM, data, len);
#endif
}

static void uart_task(void * arg)
{
    (void)arg;
    static uint8_t chunk[256];
    while (1) {
        int n = transport_read(chunk, sizeof(chunk));
        if (n > 0) {
            for (int i = 0; i < n; i++) feed_byte(chunk[i]);
        }
    }
}

static void reset_packet(void)
{
    s_buf_len = 0;
    s_expected = 0;
    s_crc = 0;
}

static void apply_timer_cb(lv_timer_t * t)
{
    (void)t;
    if (!s_pending) return;
    s_pending = false;

    s_buf[s_buf_len] = '\0';
    ESP_LOGI(TAG, "applying hot-reload document (%u bytes)", (unsigned)s_buf_len);
    lv_obj_t * root = eezml_reload(s_buf);
    if (root) {
        ESP_LOGI(TAG, "EEZML OK");
        transport_write("EEZML OK\r\n", 10);
    } else {
        ESP_LOGE(TAG, "EEZML ERR parse");
        transport_write("EEZML ERR parse\r\n", 16);
    }
    reset_packet();
}

static void feed_byte(uint8_t b)
{
    if (s_expected > 0) {
        /* inside the binary body */
        if (s_buf_len < EEZML_BUF_MAX) {
            s_buf[s_buf_len++] = (char)b;
        }
        if (s_buf_len >= s_expected) {
            /* body done: swallow until newline, then expect END line */
            s_expected = 0;
        }
        return;
    }

    /* line assembly */
    if (b == '\n') {
        s_line[s_line_len] = '\0';
        /* strip \r */
        if (s_line_len && s_line[s_line_len - 1] == '\r') s_line[--s_line_len] = '\0';

        if (strncmp(s_line, "EEZML BEGIN ", 12) == 0) {
            /* EEZML BEGIN <name> <size> — name is informational in 3a */
            char * p = s_line + 12;
            strtok(p, " ");                 /* skip name */
            char * sz = strtok(NULL, " ");
            size_t n = sz ? (size_t)atol(sz) : 0;
            if (n > 0 && n < EEZML_BUF_MAX) {
                reset_packet();
                s_expected = n;
            } else {
                ESP_LOGE(TAG, "bad BEGIN size");
                transport_write("EEZML ERR size\r\n", 15);
            }
        } else if (strncmp(s_line, "EEZML END ", 10) == 0) {
            uint32_t want = (uint32_t)strtoul(s_line + 10, NULL, 16);
            if (s_buf_len && s_expected == 0) {
                uint32_t got = crc32_buf(s_buf, s_buf_len);
                if (got == want) {
                    s_pending = true;       /* applied from the LVGL timer */
                } else {
                    ESP_LOGE(TAG, "crc mismatch got=%08lx want=%08lx",
                             (unsigned long)got, (unsigned long)want);
                    transport_write("EEZML ERR crc\r\n", 14);
                    reset_packet();
                }
            }
        }
        s_line_len = 0;
        return;
    }

    if (s_line_len < sizeof(s_line) - 1) {
        s_line[s_line_len++] = (char)b;
    } else {
        s_line_len = 0; /* overflow line: drop */
    }
}

void eezml_uart_start(void)
{
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    usb_serial_jtag_driver_config_t jcfg = {
        .rx_buffer_size = EEZML_RX_RING,
        .tx_buffer_size = 256,   /* 0 makes install fail outright */
    };
    if (usb_serial_jtag_driver_install(&jcfg) != ESP_OK) {
        ESP_LOGE(TAG, "usb_serial_jtag driver install failed");
        return;                 /* no transport rather than a crash */
    }
#else
    /* the console UART is already configured by the ROM/bootloader; install
     * only the driver + RX ring so we can read alongside ESP_LOG output */
    uart_driver_install(EEZML_UART_NUM, EEZML_RX_RING, 0, 0, NULL, 0);
    uart_set_baudrate(EEZML_UART_NUM, EEZML_UART_BAUD);
#endif

    reset_packet();
    s_pending = false;
    s_line_len = 0;

    lv_timer_create(apply_timer_cb, EEZML_APPLY_PERIOD, NULL);

    xTaskCreate(uart_task, "eezml_uart", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "hot-reload transport on %s",
             CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG ? "USB-Serial/JTAG" : ("UART" EEZML_STR(EEZML_UART_NUM)));
}
