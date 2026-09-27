# eezml_rt — eezml (uixml) runtime loader, portable core.
#
# Core sources (eezml.c + bundled expat) are plain C with no platform
# dependencies. On ESP-IDF this registers as a normal component; on any
# other MCU, add eezml.c and expat/*.c to your build and expose expat/
# and include/ on the include path.

file(GLOB EEZML_EXPAT_SRCS ${CMAKE_CURRENT_LIST_DIR}/expat/*.c)

idf_component_register(
    SRCS
        "eezml.c"
        ${EEZML_EXPAT_SRCS}
    INCLUDE_DIRS
        "include"
        "expat"
    PRIV_REQUIRES lvgl__lvgl
)
