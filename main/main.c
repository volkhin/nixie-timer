/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "alexa.h"
#include "alexa_gadgets_toolkit.h"
#include "btstack.h"
#include "btstack_debug.h"
#include "classic/sdp_util.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sdp.h"

#define TAG "main.c"

static uint8_t sdp_buffer_ota[512];
static uint8_t sdp_buffer_data[512];
static uint8_t sdp_buffer_gadget[512];
static uint8_t eir_buffer[256];

static uint8_t comm_buffer[1 << 13];
static uint16_t comm_buffer_len;
static bool inside_frame;
static bool escape;
static bool frame_parsing_error;

uint16_t rfcomm_cid;

uint8_t outgoing_buffer[1 << 13];
uint16_t outgoing_buffer_len;
uint8_t seq_id;

// TODO: don't use hardcoded Alexa MAC address
static bd_addr_t alexa_bd_addr = {0x44, 0x00, 0x49, 0x0F, 0x6A, 0x10};

uint8_t get_next_sequence_id() {
  do {
    ++seq_id;
  } while (seq_id == START_OF_FRAME || seq_id == END_OF_FRAME ||
           seq_id == ESCAPE);
  return seq_id;
}

uint16_t get_packet_checksum(uint8_t* buffer, uint16_t len) {
  if (buffer[0] != START_OF_FRAME) {
    ESP_LOGE(TAG, "Invalid packet, wrong START_OF_FRAME byte");
  }
  if (buffer[len - 1] != END_OF_FRAME) {
    ESP_LOGE(TAG, "Invalid packet, wrong END_OF_FRAME byte");
  }
  // TODO: do we need to handle escapes here?
  uint16_t checksum = buffer[1] + buffer[2];
  uint16_t index;
  bool escape = false;
  for (index = 4; index + 3 < len; ++index) {
    if (buffer[index] == ESCAPE) {
      escape = true;
      continue;
    }
    if (escape) {
      checksum += buffer[index] ^ ESCAPE;
      escape = false;
    } else {
      checksum += buffer[index];
    }
  }
  return checksum;
}

void handle_alexa_packet() {
  uint16_t checksum = get_packet_checksum(comm_buffer, comm_buffer_len);
  ESP_LOGI(TAG, "Alexa Gadgets Toolkit packet, checksum: %02x %02x",
           (checksum >> 8) & 0xff, checksum & 0xff);
  // get_next_sequence_id(); // TODO: remove?
  /* log_info_hexdump(comm_buffer, comm_buffer_len); */
  // TODO: add checks
  handle_alexa_payload(comm_buffer + 4, comm_buffer_len - 7);
}

void outgoing_data_is_ready(uint8_t* buffer, uint16_t len) {
  ESP_LOGI(TAG, "rfcomm_request_can_send_now_event");
  // TODO: find a better place for this wrapper into SPP packet
  outgoing_buffer_len = 0;
  outgoing_buffer[outgoing_buffer_len++] = START_OF_FRAME;
  outgoing_buffer[outgoing_buffer_len++] = PACKET_ID;
  outgoing_buffer[outgoing_buffer_len++] = ERROR_ID;
  outgoing_buffer[outgoing_buffer_len++] = get_next_sequence_id();

  uint16_t index;
  for (index = 0; index < len; ++index) {
    if (buffer[index] == START_OF_FRAME || buffer[index] == END_OF_FRAME ||
        buffer[index] == ESCAPE) {
      outgoing_buffer[outgoing_buffer_len++] = ESCAPE;
      outgoing_buffer[outgoing_buffer_len++] = buffer[index] ^ ESCAPE;
    } else {
      outgoing_buffer[outgoing_buffer_len++] = buffer[index];
    }
  }

  outgoing_buffer[outgoing_buffer_len++] = 0;
  outgoing_buffer[outgoing_buffer_len++] = 0;
  outgoing_buffer[outgoing_buffer_len++] = END_OF_FRAME;

  // TODO: cleaner way to write checksum
  uint16_t checksum = get_packet_checksum(outgoing_buffer, outgoing_buffer_len);
  outgoing_buffer[outgoing_buffer_len - 3] = (checksum >> 8) & 0xff;
  outgoing_buffer[outgoing_buffer_len - 2] = checksum & 0xff;
  rfcomm_request_can_send_now_event(rfcomm_cid);
}

void reset_comm_packet() {
  memset(comm_buffer, 0, sizeof(comm_buffer));
  comm_buffer_len = 0;
  inside_frame = false;
  escape = false;
  frame_parsing_error = false;
}

void handle_alexa_rfcomm_packet(uint8_t* packet, uint16_t size) {
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
            comm_buffer[comm_buffer_len++] = packet[i] ^ ESCAPE;
            escape = false;
          } else {
            comm_buffer[comm_buffer_len++] = packet[i];
          }
        }
        break;
    }
  }
}

static void handle_sdp_client_query_result(uint8_t packet_type,
                                           uint16_t channel, uint8_t* packet,
                                           uint16_t size) {
  switch (packet[0]) {
    case SDP_EVENT_QUERY_ATTRIBUTE_VALUE:
      // TODO: anything useful here?
      break;

    case SDP_EVENT_QUERY_COMPLETE:
      if (sdp_event_query_complete_get_status(packet)) {
        ESP_LOGI(TAG, "SDP query failed 0x%02x",
                 sdp_event_query_complete_get_status(packet));
        break;
      }
      ESP_LOGI(TAG, "SDP query done.");
      break;
  }
}

static void packet_handler(uint8_t packet_type, uint16_t channel,
                           uint8_t* packet, uint16_t size) {
  bd_addr_t bd_addr;

  switch (packet_type) {
    case HCI_EVENT_PACKET:
      switch (hci_event_packet_get_type(packet)) {
        case BTSTACK_EVENT_STATE:
          // BTstack activated, get started
          // TODO: get Alexa address from NVM storage
          if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING) {
            sdp_client_query_uuid16(&handle_sdp_client_query_result,
                                    alexa_bd_addr,
                                    BLUETOOTH_ATTRIBUTE_PUBLIC_BROWSE_ROOT);
          }
          break;
        case RFCOMM_EVENT_CHANNEL_OPENED:
          break;
        case RFCOMM_EVENT_CHANNEL_CLOSED:
          break;
        case RFCOMM_EVENT_CAN_SEND_NOW:
          ESP_LOGI(TAG, "RFCOMM_EVENT_CAN_SEND_NOW");
          rfcomm_send(rfcomm_cid, outgoing_buffer, outgoing_buffer_len);
          memset(outgoing_buffer, 0, sizeof(outgoing_buffer));
          outgoing_buffer_len = 0;
          seq_id = 0;
          break;
        case RFCOMM_EVENT_INCOMING_CONNECTION:
          rfcomm_event_incoming_connection_get_bd_addr(packet, bd_addr);
          rfcomm_cid = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
          ESP_LOGI(TAG, "Accepting RFCOMM connection from %s, cid: %d",
                   bd_addr_to_str(bd_addr), rfcomm_cid);
          rfcomm_accept_connection(rfcomm_cid);
          break;
      }
      break;

    case RFCOMM_DATA_PACKET:
      ESP_LOGI(TAG, "Got RFCOMM data on channel %d", channel);
      handle_alexa_rfcomm_packet(packet, size);
      break;

    default:
      ESP_LOGI(TAG, "Unknown packet type: %d", packet_type);
  }
}

static btstack_packet_callback_registration_t hci_event_callback_registration;

int btstack_main(int argc, const char* argv[]) {
  /* UNUSED(argc); */
  /* UNUSED(argv); */

  ESP_LOGI(TAG, "btstack_main");

  memset(eir_buffer, 0, sizeof(eir_buffer));
  build_eir(eir_buffer, "Volkhin Nixie Timer", 0x0000, 0xffff);
  gap_set_extended_inquiry_response(eir_buffer);
  hci_event_callback_registration.callback = &packet_handler;
  hci_add_event_handler(&hci_event_callback_registration);

  l2cap_init();
  rfcomm_init();
  /* rfcomm_register_service(packet_handler, RFCOMM_OTA_CHANNEL, 0xffff); */
  rfcomm_register_service(packet_handler, RFCOMM_DATA_CHANNEL, 0xffff);

  // SDP
  sdp_init();

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

  // Alexa
  register_send_data_callback(&outgoing_data_is_ready);

  xTaskCreate(&timer_task, "timer_task", TIMER_TASK_STACK_SIZE, NULL, 5, NULL);

  // turn Bluetooth on
  gap_discoverable_control(1);
  gap_set_local_name("ESP32 device");
  hci_power_control(HCI_POWER_ON);

  return 0;
}
