#include <string.h>

#include "bluetooth_data_types.h"
#include "bluetooth_sdp.h"
#include "btstack_debug.h"
#include "classic/sdp_util.h"
#include "sdp.h"

void sdp_create_ota(uint8_t *service, uint32_t service_record_handle,
                    int rfcomm_channel, const char *name) {
  uint8_t *attribute;
  de_create_sequence(service);

  // 0x0000 "Service Record Handle"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_SERVICE_RECORD_HANDLE);
  de_add_number(service, DE_UINT, DE_SIZE_32, service_record_handle);

  // 0x0001 "Service Class ID List"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_SERVICE_CLASS_ID_LIST);
  attribute = de_push_sequence(service);
  {
    de_add_number(attribute, DE_UUID, DE_SIZE_16,
                  BLUETOOTH_SERVICE_CLASS_SERIAL_PORT);
  }
  de_pop_sequence(service, attribute);

  // 0x0004 "Protocol Descriptor List"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST);
  attribute = de_push_sequence(service);
  {
    uint8_t *l2cpProtocol = de_push_sequence(attribute);
    {
      de_add_number(l2cpProtocol, DE_UUID, DE_SIZE_16,
                    BLUETOOTH_PROTOCOL_L2CAP);
    }
    de_pop_sequence(attribute, l2cpProtocol);

    uint8_t *rfcomm = de_push_sequence(attribute);
    {
      de_add_number(rfcomm, DE_UUID, DE_SIZE_16,
                    BLUETOOTH_PROTOCOL_RFCOMM);  // rfcomm_service
      de_add_number(rfcomm, DE_UINT, DE_SIZE_8,
                    rfcomm_channel);  // rfcomm channel
    }
    de_pop_sequence(attribute, rfcomm);
  }
  de_pop_sequence(service, attribute);

  // 0x0005 "Public Browse Group"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_BROWSE_GROUP_LIST);  // public browse group
  attribute = de_push_sequence(service);
  {
    de_add_number(attribute, DE_UUID, DE_SIZE_16,
                  BLUETOOTH_ATTRIBUTE_PUBLIC_BROWSE_ROOT);
  }
  de_pop_sequence(service, attribute);

  // 0x0100 "ServiceName"
  de_add_number(service, DE_UINT, DE_SIZE_16, 0x0100);
  de_add_data(service, DE_STRING, strlen(name), (uint8_t *)name);
}

void sdp_create_data(uint8_t *service, uint32_t service_record_handle,
                     int rfcomm_channel, const char *name) {
  uint8_t *attribute;
  de_create_sequence(service);

  // 0x0000 "Service Record Handle"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_SERVICE_RECORD_HANDLE);
  de_add_number(service, DE_UINT, DE_SIZE_32, service_record_handle);

  // 0x0001 "Service Class ID List"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_SERVICE_CLASS_ID_LIST);
  attribute = de_push_sequence(service);
  {
    de_add_number(attribute, DE_UUID, DE_SIZE_16,
                  BLUETOOTH_SERVICE_CLASS_GENERIC_NETWORKING);
  }
  de_pop_sequence(service, attribute);

  // 0x0004 "Protocol Descriptor List"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST);
  attribute = de_push_sequence(service);
  {
    uint8_t *l2cpProtocol = de_push_sequence(attribute);
    {
      de_add_number(l2cpProtocol, DE_UUID, DE_SIZE_16,
                    BLUETOOTH_PROTOCOL_L2CAP);
    }
    de_pop_sequence(attribute, l2cpProtocol);

    uint8_t *rfcomm = de_push_sequence(attribute);
    {
      de_add_number(rfcomm, DE_UUID, DE_SIZE_16,
                    BLUETOOTH_PROTOCOL_RFCOMM);  // rfcomm_service
      de_add_number(rfcomm, DE_UINT, DE_SIZE_8,
                    rfcomm_channel);  // rfcomm channel
    }
    de_pop_sequence(attribute, rfcomm);
  }
  de_pop_sequence(service, attribute);

  // 0x0005 "Public Browse Group"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_BROWSE_GROUP_LIST);  // public browse group
  attribute = de_push_sequence(service);
  {
    de_add_number(attribute, DE_UUID, DE_SIZE_16,
                  BLUETOOTH_ATTRIBUTE_PUBLIC_BROWSE_ROOT);
  }
  de_pop_sequence(service, attribute);

  // 0x0100 "ServiceName"
  de_add_number(service, DE_UINT, DE_SIZE_16, 0x0100);
  de_add_data(service, DE_STRING, strlen(name), (uint8_t *)name);
}

void sdp_create_gadget(uint8_t *service, uint32_t service_record_handle,
                       int rfcomm_channel, const char *name) {
  uint8_t *attribute;
  de_create_sequence(service);

  // 0x0000 "Service Record Handle"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_SERVICE_RECORD_HANDLE);
  de_add_number(service, DE_UINT, DE_SIZE_32, service_record_handle);

  // 0x0001 "Service Class ID List"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_SERVICE_CLASS_ID_LIST);
  attribute = de_push_sequence(service);
  {
    // TODO
    de_add_uuid128(attribute, gadget_uuid);
    /* de_add_number(attribute, DE_UUID, DE_SIZE_16, */
    /* BLUETOOTH_SERVICE_CLASS_GENERIC_NETWORKING); */
  }
  de_pop_sequence(service, attribute);

  // 0x0004 "Protocol Descriptor List"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_PROTOCOL_DESCRIPTOR_LIST);
  attribute = de_push_sequence(service);
  {
    uint8_t *l2cpProtocol = de_push_sequence(attribute);
    {
      de_add_number(l2cpProtocol, DE_UUID, DE_SIZE_16,
                    BLUETOOTH_PROTOCOL_L2CAP);
    }
    de_pop_sequence(attribute, l2cpProtocol);

    uint8_t *rfcomm = de_push_sequence(attribute);
    {
      de_add_number(rfcomm, DE_UUID, DE_SIZE_16,
                    BLUETOOTH_PROTOCOL_RFCOMM);  // rfcomm_service
      de_add_number(rfcomm, DE_UINT, DE_SIZE_8,
                    rfcomm_channel);  // rfcomm channel
    }
    de_pop_sequence(attribute, rfcomm);
  }
  de_pop_sequence(service, attribute);

  // 0x0005 "Public Browse Group"
  de_add_number(service, DE_UINT, DE_SIZE_16,
                BLUETOOTH_ATTRIBUTE_BROWSE_GROUP_LIST);  // public browse group
  attribute = de_push_sequence(service);
  {
    de_add_number(attribute, DE_UUID, DE_SIZE_16,
                  BLUETOOTH_ATTRIBUTE_PUBLIC_BROWSE_ROOT);
  }
  de_pop_sequence(service, attribute);

  // 0x0100 "ServiceName"
  de_add_number(service, DE_UINT, DE_SIZE_16, 0x0100);
  de_add_data(service, DE_STRING, strlen(name), (uint8_t *)name);

  // TODO: make sure it's exactly 10 bytes of data
  // 0x00ff manufacturer-specific data section
  de_add_number(service, DE_UINT, DE_SIZE_16, 0x00ff);
  attribute = de_push_sequence(service);
  {
    de_add_number(attribute, DE_UINT, DE_SIZE_16,
                  0xffff);  // Bluetooth Vendor ID
    de_add_number(attribute, DE_UINT, DE_SIZE_16,
                  0x0000);  // Bluetooth Product ID
    de_add_number(attribute, DE_UINT, DE_SIZE_16, 0x0171);  // Amazon SIG VID
    de_add_number(attribute, DE_UINT, DE_SIZE_32,
                  0xfe151510);  // Amazon Gadget UUID
  }
  de_pop_sequence(service, attribute);
}

void build_eir(uint8_t *buffer, const char *name, uint16_t pid, uint16_t vid) {
  uint8_t *ptr = buffer;

  // 0x09 Complete Local Name
  uint8_t name_len = strlen(name);
  *ptr++ = name_len + 1;
  *ptr++ = BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME;
  memcpy(ptr, name, name_len);
  ptr += name_len;

  // 0x06 Incomplete List of 128-bit Service Class UUIDs
  *ptr++ = 1 + 16;  // 1 byte for DataType + 128 bit per each uuid
  *ptr++ = BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS;
  for (int8_t i = 15; i >= 0; --i) {
    *ptr++ = gadget_uuid[i];
  }

  // 0xFF Manufacturer Specific Data
  *ptr++ = 1 + 10;  // 1 byte for DataType + 10 for custom data
  *ptr++ = BLUETOOTH_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA;
  *ptr++ = vid & 0xff;
  *ptr++ = (vid >> 8) & 0xff;
  *ptr++ = pid & 0xff;
  *ptr++ = (pid >> 8) & 0xff;
  *ptr++ = AMAZON_SIG_VID & 0xff;
  *ptr++ = AMAZON_SIG_VID >> 8;
  *ptr++ = GADGET_UUID_32 & 0xff;
  *ptr++ = (GADGET_UUID_32 >> 8) & 0xff;
  *ptr++ = (GADGET_UUID_32 >> 16) & 0xff;
  *ptr++ = (GADGET_UUID_32 >> 24) & 0xff;
}
