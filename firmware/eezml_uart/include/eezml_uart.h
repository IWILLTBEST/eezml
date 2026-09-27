#ifndef EEZML_UART_H
#define EEZML_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/** Start the hot-reload transport: UART0 RX task + LVGL apply timer.
 *  Call once from app_main (after bsp_display_start, before or after
 *  eezml_create — either order works). */
void eezml_uart_start(void);

#ifdef __cplusplus
}
#endif

#endif /* EEZML_UART_H */
