#include <stdint.h>

#define TIMER_TASK_STACK_SIZE 16<<10

typedef void (*send_data_callback_t)(uint8_t* buffer, uint16_t len);

void handle_alexa_payload(uint8_t* buffer, uint16_t len);
void register_send_data_callback(send_data_callback_t callback);
void timer_task(void *pvParameter);

send_data_callback_t send_data_callback;
