set(COMPONENT_ADD_INCLUDEDIRS .)

# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES "spi_flash")
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS ./esp_spiffs.c
                   ./list.c
                   ./mutex.c
                   ./spiffs_cache.c
                   ./spiffs_check.c
                   ./spiffs_gc.c
                   ./spiffs_hydrogen.c
                   ./spiffs_nucleus.c
                   ./spiffs_vfs.c)

register_component()
