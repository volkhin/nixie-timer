/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "btstack_debug.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "alexa_gadgets_toolkit.h"
#include "btstack.h"
#include "classic/sdp_util.h"
#include "sdp.h"

#define MAIN_TAG "MAIN"

static uint8_t sdp_buffer_ota[512];
static uint8_t sdp_buffer_data[512];
static uint8_t sdp_buffer_gadget[512];
static uint8_t eir_buffer[256];

static uint8_t comm_buffer[1024];
static uint16_t comm_buffer_len;
static bool inside_frame;
static bool escape;
static bool frame_parsing_error;

void handle_alexa_packet() {
  ESP_LOGI(MAIN_TAG, "Alexa Gadgets Toolkit packet");
  log_info_hexdump(comm_buffer, comm_buffer_len);
}

void reset_comm_packet() {
  memset(comm_buffer, 0, sizeof(comm_buffer));
  comm_buffer_len = 0;
  inside_frame = false;
  escape = false;
  frame_parsing_error = false;
}

void handle_alexa_rfcomm_packet(uint8_t* packet, uint16_t size) {
  log_info_hexdump(packet, size);

  for (uint16_t i = 0; i < size; ++i) {
    if (comm_buffer_len >= sizeof(comm_buffer)) {
      frame_parsing_error = true;
    }

    if (packet[i] == START_OF_FRAME) {
      reset_comm_packet();
    }

    if (frame_parsing_error) {
      continue;
    }

    switch (packet[i]) {
      case START_OF_FRAME:
        if (inside_frame) {
          // END_OF_FRAME was expected first
          frame_parsing_error = true;
        } else {
          comm_buffer[comm_buffer_len++] = packet[i];
          inside_frame = true;
        }
        break;

      case END_OF_FRAME:
        if (!inside_frame) {
          // START_OF_FRAME was expected first
          frame_parsing_error = true;
        } else {
          comm_buffer[comm_buffer_len++] = packet[i];
          inside_frame = false;
          handle_alexa_packet();
        }
        break;

      case ESCAPE:
        escape = true;

      default:
        if (!inside_frame) {
          frame_parsing_error = true;
        } else {
          if (escape) {
            packet[i] ^= ESCAPE;
            escape = false;
          }
          comm_buffer[comm_buffer_len++] = packet[i];
        }
        break;
    }
  }

  // TODO: static buffer?
  char* buffer = malloc(size + 1);
  memcpy(buffer, packet, size);
  buffer[size] = 0;
  free(buffer);
}

static void packet_handler(uint8_t packet_type, uint16_t channel,
                           uint8_t* packet, uint16_t size) {
  /* ESP_LOGI(MAIN_TAG, "handle packet (type: %d, channel: %d, size: %d)",
   * packet_type, channel, size); */

  switch (packet_type) {
    case HCI_EVENT_PACKET:
      switch (hci_event_packet_get_type(packet)) {
        uint16_t cid;
        case RFCOMM_EVENT_INCOMING_CONNECTION:
          cid = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
          ESP_LOGI(MAIN_TAG, "Accepting RFCOMM connection, cid: %d", cid);
          rfcomm_accept_connection(cid);
          break;
        default:
          /* ESP_LOGI(MAIN_TAG, "HCI_EVENT_PACKET"); */
          break;
      }
      break;

    case RFCOMM_DATA_PACKET:
      ESP_LOGI(MAIN_TAG, "Got RFCOMM data on channel %d", channel);
      handle_alexa_rfcomm_packet(packet, size);
      break;

    default:
      ESP_LOGI(MAIN_TAG, "Unknown packet type: %d", packet_type);
  }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

int btstack_main(int argc, const char* argv[]) {
  /* UNUSED(argc); */
  /* UNUSED(argv); */

  ESP_LOGI(MAIN_TAG, "btstack_main");

  memset(eir_buffer, 0, sizeof(eir_buffer));
  build_eir(eir_buffer, "Volkhin Nixie Timer", 0x0000, 0xffff);
  log_info_hexdump(eir_buffer, 240);
  gap_set_extended_inquiry_response(eir_buffer);
  hci_event_callback_registration.callback = &packet_handler;
  hci_add_event_handler(&hci_event_callback_registration);

  l2cap_init();
  rfcomm_init();
  /* rfcomm_register_service(packet_handler, RFCOMM_OTA_CHANNEL, 0xffff); */
  rfcomm_register_service(packet_handler, RFCOMM_DATA_CHANNEL, 0xffff);

  memset(sdp_buffer_ota, 0, sizeof(sdp_buffer_ota));
  sdp_create_ota(sdp_buffer_ota, RFCOMM_OTA_HANDLE, RFCOMM_OTA_CHANNEL,
                 RFCOMM_OTA_NAME);
  sdp_register_service(sdp_buffer_ota);

  memset(sdp_buffer_data, 0, sizeof(sdp_buffer_data));
  sdp_create_data(sdp_buffer_data, RFCOMM_DATA_HANDLE, RFCOMM_DATA_CHANNEL,
                  RFCOMM_DATA_NAME);
  sdp_register_service(sdp_buffer_data);

  memset(sdp_buffer_gadget, 0, sizeof(sdp_buffer_gadget));
  sdp_create_gadget(sdp_buffer_gadget, RFCOMM_GADGET_HANDLE,
                    RFCOMM_GADGET_CHANNEL, RFCOMM_GADGET_NAME);
  sdp_register_service(sdp_buffer_gadget);

  sdp_init();

  gap_discoverable_control(1);
  gap_set_local_name("ESP32 device");

  hci_power_control(HCI_POWER_ON);

  return 0;
}
