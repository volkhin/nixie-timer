#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "alertsDeleteAlertDirective.pb.h"
#include "alertsSetAlertDirective.pb.h"
#include "alexa.h"
#include "alexaDiscoveryDiscoverDirective.pb.h"
#include "alexaDiscoveryDiscoverResponseEvent.pb.h"
#include "alexaGadgetSpeechDataSpeechmarksDirective.pb.h"
#include "alexaGadgetStateListenerStateUpdateDirective.pb.h"
#include "btstack_debug.h"
#include "directiveParser.pb.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_system.h"
#include "eventParser.pb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "notificationsSetIndicatorDirective.pb.h"
#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "spiffs_vfs.h"
#include "tft.h"
#include "tftspi.h"

static const char* TAG = "alexa.c";

uint8_t out_buffer[1000];
uint16_t out_buffer_len;
static time_t timer;
static char timer_token[100];

// Define which spi bus to use TFT_VSPI_HOST or TFT_HSPI_HOST
#define SPI_BUS TFT_HSPI_HOST

void create_discovery_response() {
  /*
  {
      "event": {
          "header": {
              "namespace": "Alexa.Discovery",
              "name": "Discover.Response",
              "messageId": ""
          },
          "payload": {
              "endpoints": [{
                  "endpointId": "test id",
                  "friendlyName": "friendly name",
                  "capabilities": [
                  {
                      "type": "test type 1",
                      "interface": "Test interface 1",
                      "version": "1.0"
                  },
                  {
                      "type": "test type 2",
                      "interface": "Test interface 2",
                      "version": "1.0"
                  },
                  {
                      "type": "test type 3",
                      "interface": "Test interface 3",
                      "version": "1.1"
                  }],
                  "additionalIdentification": {
                      "firmwareVersion": "19",
                      "deviceToken": "xxxxxxxxx",
                      "deviceTokenEncryptionType": "yyy",
                      "amazonDeviceType": "aabbccd",
                      "modelName": "mock model name",
                      "radioAddress": "1234567890"
                  }
              }]
          }
      }
  }
  */
  ESP_LOGI(TAG, "Creating discover response event");
  pb_ostream_t stream = pb_ostream_from_buffer(out_buffer, sizeof(out_buffer));
  static alexaDiscovery_DiscoverResponseEventProto env =
      alexaDiscovery_DiscoverResponseEventProto_init_zero;

  strcpy(env.event.header.namespace, "Alexa.Discovery");
  strcpy(env.event.header.name, "Discover.Response");

  env.event.payload.endpoints_count = 1;
  strcpy(env.event.payload.endpoints[0].endpointId, "A3ES37RU3TCYA7");
  strcpy(env.event.payload.endpoints[0].friendlyName, "NixieTimer");
  strcpy(env.event.payload.endpoints[0].description, "Nixie Timer");
  strcpy(env.event.payload.endpoints[0].manufacturerName, "Artem Volkhin");

  env.event.payload.endpoints[0].capabilities_count = 2;
  strcpy(env.event.payload.endpoints[0].capabilities[0].type, "AlexaInterface");
  strcpy(env.event.payload.endpoints[0].capabilities[0].interface, "Alerts");
  strcpy(env.event.payload.endpoints[0].capabilities[0].version, "1.1");
  strcpy(env.event.payload.endpoints[0].capabilities[1].type, "AlexaInterface");
  strcpy(env.event.payload.endpoints[0].capabilities[1].interface,
         "Alexa.Gadget.StateListener");
  strcpy(env.event.payload.endpoints[0].capabilities[1].version, "1.0");
  env.event.payload.endpoints[0]
      .capabilities[1]
      .configuration.supportedTypes_count = 3;
  strcpy(env.event.payload.endpoints[0]
             .capabilities[1]
             .configuration.supportedTypes[0]
             .name,
         "timers");
  strcpy(env.event.payload.endpoints[0]
             .capabilities[1]
             .configuration.supportedTypes[1]
             .name,
         "timeinfo");
  strcpy(env.event.payload.endpoints[0]
             .capabilities[1]
             .configuration.supportedTypes[2]
             .name,
         "wakeword");

  strcpy(
      env.event.payload.endpoints[0].additionalIdentification.firmwareVersion,
      "0");
  strcpy(env.event.payload.endpoints[0].additionalIdentification.deviceToken,
         "7d407b1bc2cc280c6ba93078c25375b619f6c32fe7b26494c102c323dd926fcf");
  strcpy(env.event.payload.endpoints[0]
             .additionalIdentification.deviceTokenEncryptionType,
         "1");
  strcpy(
      env.event.payload.endpoints[0].additionalIdentification.amazonDeviceType,
      "A3ES37RU3TCYA7");
  strcpy(env.event.payload.endpoints[0].additionalIdentification.modelName,
         "NixieTimer");
  strcpy(env.event.payload.endpoints[0].additionalIdentification.radioAddress,
         "3C71BF9ABE66");

  bool status = pb_encode(
      &stream, alexaDiscovery_DiscoverResponseEventProto_fields, &env);
  if (!status) {
    ESP_LOGE(TAG, "Error encoding message: %s", PB_GET_ERROR(&stream));
    return;
  }

  out_buffer_len = stream.bytes_written;
  send_data_callback(out_buffer, out_buffer_len);
  memset(out_buffer, 0, sizeof(out_buffer));
  out_buffer_len = 0;
}

void handle_discovery_request(pb_istream_t stream) {
  static alexaDiscovery_DiscoverDirectiveProto envelope =
      alexaDiscovery_DiscoverDirectiveProto_init_default;
  pb_decode(&stream, alexaDiscovery_DiscoverDirectiveProto_fields, &envelope);
  printf("scope type: %s\n", envelope.directive.payload.scope.type);
  printf("scope token: %s\n", envelope.directive.payload.scope.token);

  create_discovery_response();
}

void handle_time_info(char* current_time) {
  static struct tm tm;
  strptime(current_time, "%Y-%m-%dT%H:%M:%S", &tm);
  time_t t = mktime(&tm);
  struct timeval now = {.tv_sec = t};
  settimeofday(&now, NULL);

  ESP_LOGI(TAG, "time info: %d %d %d %d %d %d, %ld", tm.tm_year, tm.tm_mon,
           tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, t);
}

void set_timer(char* token, char* scheduled_time) {
  static struct tm tm;
  strptime(scheduled_time, "%Y-%m-%dT%H:%M:%S", &tm);
  timer = mktime(&tm);
  memset(timer_token, 0, sizeof(timer_token));
  strcpy(timer_token, token);
  ESP_LOGI(TAG, "timer: %s %d %d %d %d %d %d", token, tm.tm_year, tm.tm_mon,
           tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void cancel_timer() {
  ESP_LOGI(TAG, "Cancelling timer");
  memset(timer_token, 0, sizeof(timer_token));
  timer = 0;
}

void handle_set_alert(pb_istream_t stream) {
  static alerts_SetAlertDirectiveProto envelope =
      alerts_SetAlertDirectiveProto_init_default;
  pb_decode(&stream, alerts_SetAlertDirectiveProto_fields, &envelope);
  printf("alert type: %s\n", envelope.directive.payload.type);
  printf("alert token: %s\n", envelope.directive.payload.token);
  printf("alert scheduled time: %s\n",
         envelope.directive.payload.scheduledTime);

  if (0 == strcmp(envelope.directive.payload.type, "TIMER")) {
    set_timer(envelope.directive.payload.token,
                     envelope.directive.payload.scheduledTime);
  }
}

void handle_delete_alert(pb_istream_t stream) {
  static alerts_DeleteAlertDirectiveProto envelope =
      alerts_DeleteAlertDirectiveProto_init_default;
  pb_decode(&stream, alerts_SetAlertDirectiveProto_fields, &envelope);
  printf("alert token: %s\n", envelope.directive.payload.token);

  if (0 == strcmp(timer_token, envelope.directive.payload.token)) {
    cancel_timer();
  }
}

void handle_alexa_payload(uint8_t* buffer, uint16_t len) {
  ESP_LOGI(TAG, "Parsing Directive");
  static directive_DirectiveParserProto envelope =
      directive_DirectiveParserProto_init_default;

  pb_istream_t stream = pb_istream_from_buffer(buffer, len);
  pb_decode(&stream, directive_DirectiveParserProto_fields, &envelope);
  stream = pb_istream_from_buffer(buffer, len);

  ESP_LOGI(TAG, "name = %s, namespace=%s", envelope.directive.header.name,
           envelope.directive.header.namespace);

  char* name = envelope.directive.header.name;
  char* namespace = envelope.directive.header.namespace;
  if (0 == strcmp(name, "SetIndicator") &&
      (0 == strcmp(namespace, "Notifications"))) {
    // Parse the notification payload now
    static notifications_SetIndicatorDirectiveProto notifications_envelope =
        notifications_SetIndicatorDirectiveProto_init_default;
    pb_decode(&stream, notifications_SetIndicatorDirectiveProto_fields,
              &notifications_envelope);

    printf("visualIndicator:%d, audioIndicator=%d, assetId=%s, url=%s\n",
           notifications_envelope.directive.payload.persistVisualIndicator,
           notifications_envelope.directive.payload.playAudioIndicator,
           notifications_envelope.directive.payload.asset.assetId,
           notifications_envelope.directive.payload.asset.url);
  } else if (0 == strcmp(name, "Discover") &&
             (0 == strcmp(namespace, "Alexa.Discovery"))) {
    handle_discovery_request(stream);
  } else if (0 == strcmp(name, "SetAlert") &&
             0 == strcmp(namespace, "Alerts")) {
    handle_set_alert(stream);
  } else if (0 == strcmp(name, "DeleteAlert") &&
             0 == strcmp(namespace, "Alerts")) {
    handle_delete_alert(stream);
  } else if (0 == strcmp(name, "StateUpdate") &&
             (0 == strcmp(namespace, "Alexa.Gadget.StateListener"))) {
    static alexaGadgetStateListener_StateUpdateDirectiveProto envelope =
        alexaGadgetStateListener_StateUpdateDirectiveProto_init_default;
    pb_decode(&stream,
              alexaGadgetStateListener_StateUpdateDirectiveProto_fields,
              &envelope);
    int states_count = envelope.directive.payload.states_count;
    for (int i = 0; i < states_count; ++i) {
      char* name = envelope.directive.payload.states[i].name;
      char* value = envelope.directive.payload.states[i].value;
      printf("state name: %s\n", name);
      printf("state value: %s\n", value);
      if (0 == strcmp(name, "timeinfo")) {
        handle_time_info(value);
      }
    }
  } else if (0 == strcmp(name, "Speechmarks") &&
             (0 == strcmp(namespace, "Alexa.Gadget.SpeechData"))) {
    static alexaGadgetSpeechData_SpeechmarksDirectiveProto envelope =
        alexaGadgetSpeechData_SpeechmarksDirectiveProto_init_default;
    pb_decode(&stream, alexaGadgetSpeechData_SpeechmarksDirectiveProto_fields,
              &envelope);
    int speechmarks_count = envelope.directive.payload.speechmarksData_count;
    printf("player offset: %d\n",
           envelope.directive.payload.playerOffsetInMilliSeconds);
    for (int i = 0; i < speechmarks_count; ++i) {
      printf("speechmark type: %s\n",
             envelope.directive.payload.speechmarksData[i].type);
      printf("speechmark value: %s\n",
             envelope.directive.payload.speechmarksData[i].value);
      printf("speechmark start offset: %d\n",
             envelope.directive.payload.speechmarksData[i]
                 .startOffsetInMilliSeconds);
    }
  } else {
    ESP_LOGE(TAG, "Error: unknown directive. Name: %s, namespace: %s", name,
             namespace);
  }
}

void register_send_data_callback(send_data_callback_t callback) {
  send_data_callback = callback;
}

void timer_task(void* pvParameter) {
  ESP_LOGI(TAG, "timer_task started");
  esp_err_t ret;

  tft_disp_type = DEFAULT_DISP_TYPE;
  _width = DEFAULT_TFT_DISPLAY_WIDTH;    // smaller dimension
  _height = DEFAULT_TFT_DISPLAY_HEIGHT;  // larger dimension
  max_rdclock = 8000000;

  // === Pins MUST be initialized before SPI interface initialization ===
  TFT_PinsInit();

  spi_lobo_device_handle_t spi;
  spi_lobo_bus_config_t buscfg = {
      .miso_io_num = PIN_NUM_MISO,  // set SPI MISO pin
      .mosi_io_num = PIN_NUM_MOSI,  // set SPI MOSI pin
      .sclk_io_num = PIN_NUM_CLK,   // set SPI CLK pin
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 6 * 1024,
  };
  spi_lobo_device_interface_config_t devcfg = {
      .clock_speed_hz = 8000000,          // Initial clock out at 8 MHz
      .mode = 0,                          // SPI mode 0
      .spics_io_num = -1,                 // we will use external CS pin
      .spics_ext_io_num = PIN_NUM_CS,     // external CS pin
      .flags = LB_SPI_DEVICE_HALFDUPLEX,  // ALWAYS SET  to HALF DUPLEX MODE!!
                                          // for display spi
  };

  vTaskDelay(500 / portTICK_RATE_MS);

  ESP_LOGI("SPI", "Pins used: miso=%d, mosi=%d, sck=%d, cs=%d", PIN_NUM_MISO,
           PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_CS);

  // ==== Initialize the SPI bus and attach the LCD to the SPI bus ====
  ret = spi_lobo_bus_add_device(SPI_BUS, &buscfg, &devcfg, &spi);
  assert(ret == ESP_OK);
  ESP_ERROR_CHECK(ret);
  ESP_LOGI("SPI", "display device added to spi bus (%d)", SPI_BUS);
  disp_spi = spi;

  // ==== Test select/deselect ====
  ret = spi_lobo_device_select(spi, 1);
  ESP_ERROR_CHECK(ret);
  ret = spi_lobo_device_deselect(spi);
  ESP_ERROR_CHECK(ret);

  ESP_LOGI("SPI", "attached display device, speed=%u", spi_lobo_get_speed(spi));
  ESP_LOGI("SPI", "bus uses native pins: %s",
           spi_lobo_uses_native_pins(spi) ? "true" : "false");

  // ==== Initialize the Display ====
  ESP_LOGI("SPI", "display init...");
  TFT_display_init();
  ESP_LOGI("SPI", "ok");

  // ---- Detect maximum read speed ----
  max_rdclock = find_rd_speed();
  ESP_LOGI("SPI", "Max rd speed = %u", max_rdclock);

  // ==== Set SPI clock used for display operations ====
  spi_lobo_set_speed(spi, DEFAULT_SPI_CLOCK);
  ESP_LOGI("SPI", "Changed speed to %u", spi_lobo_get_speed(spi));

  TFT_setGammaCurve(DEFAULT_GAMMA_CURVE);
  TFT_setRotation(PORTRAIT);
  TFT_setFont(DEFAULT_FONT, NULL);
  TFT_resetclipwin();

  TFT_fillScreen(TFT_BLACK);
  TFT_print("Hello world!", CENTER, CENTER);

  for (;;) {
    static struct timeval now;
    gettimeofday(&now, NULL);
    static char timer_output[10];
    if (timer != 0 && now.tv_sec != 0) {
      time_t time_left = timer - now.tv_sec;
      if (time_left < 0) {
        time_left = 0;
      } else if (time_left > 99 * 60 + 99) {
        time_left = 99 * 60 + 99;
      }
      sprintf(timer_output, "%02ld:%02ld", time_left / 60, time_left % 60);
    } else {
      sprintf(timer_output, "No timer");
    }
    TFT_fillScreen(TFT_BLACK);
    TFT_print(timer_output, CENTER, CENTER);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
