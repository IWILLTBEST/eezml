# eezml_uart — ESP-IDF port: hot-reload transport over console UART.
# Depends on the portable eezml_rt core only for the reload API.

idf_component_register(
    SRCS "eezml_uart.c"
    INCLUDE_DIRS "include"
    REQUIRES eezml_rt lvgl__lvgl
    PRIV_REQUIRES esp_driver_uart esp_driver_usb_serial_jtag esp_timer
)
