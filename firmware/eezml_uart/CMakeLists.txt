# eezml_uart — ESP-IDF port: hot-reload transport over console UART,
# plus flash persistence of the last pushed document ("storage" partition).

idf_component_register(
    SRCS "eezml_uart_persist.c" "eezml_uart.c"
    INCLUDE_DIRS "include"
    REQUIRES eezml_rt lvgl__lvgl
    PRIV_REQUIRES esp_driver_uart esp_driver_usb_serial_jtag esp_timer esp_partition
)
