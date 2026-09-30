#ifndef EEZML_UART_PERSIST_H
#define EEZML_UART_PERSIST_H

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Store one document in the "storage" flash partition (overwrites any
   previous one). */
esp_err_t eezml_uart_persist_store(const char * name, const char * xml, size_t len);

/* Load the persisted document. Returns a malloc'd NUL-terminated string
   (caller frees) or NULL when nothing valid is stored. */
char * eezml_uart_persist_load(char * name_out, size_t name_sz);

/* Forget the persisted document (back to the compiled-in UI). */
esp_err_t eezml_uart_persist_wipe(void);

#ifdef __cplusplus
}
#endif

#endif /* EEZML_UART_PERSIST_H */
