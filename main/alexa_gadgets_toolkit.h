#include <stdint.h>

#define GADGET_NAME "Volkhin Gadget Name"

static int const RFCOMM_OTA_HANDLE = 0x10002;
static int const RFCOMM_OTA_CHANNEL = 2;
static const char *RFCOMM_OTA_NAME = "SPP Port OTA";

static int const RFCOMM_DATA_HANDLE = 0x10000;
static int const RFCOMM_DATA_CHANNEL = 4;
static const char *RFCOMM_DATA_NAME = "SPP Port DATA";

static int const RFCOMM_GADGET_HANDLE = 0x10001;
static int const RFCOMM_GADGET_CHANNEL = 4;
static const char *RFCOMM_GADGET_NAME = GADGET_NAME;


typedef enum {
  START_OF_FRAME = 0xf0,
  END_OF_FRAME = 0xf1,
  ESCAPE = 0xf2
} alexa_packet_component_t;

static const uint8_t PACKET_ID = 0x02;
static const uint8_t ERROR_ID = 0x00;
