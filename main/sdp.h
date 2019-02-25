#pragma once

// Magic UUID for Alexa Gadgets: 6088D2B3-983A-4EED-9F94-5AD1256816B7
static uint8_t gadget_uuid[16] = {0x60, 0x88, 0xd2, 0xb3, 0x98, 0x3a,
                                  0x4e, 0xed, 0x9f, 0x94, 0x5a, 0xd1,
                                  0x25, 0x68, 0x16, 0xb7};

static uint8_t gadget_uuid_reverse[16] = {
    0xb7, 0x16, 0x68, 0x25, 0xd1, 0x5a, 0x94, 0x9f,
    0xed, 0x4e, 0x3a, 0x98, 0xb3, 0xd2, 0x88, 0x60,
};

static const uint32_t GADGET_UUID_32 = 0xFE151510;
static const uint16_t AMAZON_SIG_VID = 0x0171;

void sdp_create_ota(uint8_t *service, uint32_t service_record_handle,
                    int rfcomm_channel, const char *name);
void sdp_create_data(uint8_t *service, uint32_t service_record_handle,
                     int rfcomm_channel, const char *name);
void sdp_create_gadget(uint8_t *service, uint32_t service_record_handle,
                       int rfcomm_channel, const char *name);
void build_eir(uint8_t *buffer, const char *name, uint16_t pid, uint16_t vid);
