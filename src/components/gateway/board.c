#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "board.h"

#define TAG "BOARD"

static const uint8_t pins[] = {GPIO_NUM_33, GPIO_NUM_32,
                               GPIO_NUM_16};
static int status_eth = 0;
static int status_mqtt = 0;
static int status_error = 0;


void board_boot() {
  gpio_set_level(pins[0], 1);
}

void board_eth() {
  status_eth = 1;
}

void board_err(int err) {
  status_error = err;
  ESP_LOGW(TAG, "Status error: %d", err);
}

void board_mqtt() {
  ESP_LOGW(TAG, "Status MQTT activated");
  status_mqtt = 1;
}

static int blink_switch = 0;
void led_task()
{
  for(;;) {
    blink_switch = blink_switch ? 0 : 1;

    if(status_eth && !status_mqtt) {
      gpio_set_level(pins[1], blink_switch);
    }

    if(status_mqtt) {
      gpio_set_level(pins[1], 1);
    }

    gpio_set_level(pins[2], status_error == 1);

    vTaskDelay(100);
  }

}

void setup_pin(uint8_t pin) {
  gpio_pad_select_gpio(pin);
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);
  gpio_set_level(pin, 0);
}

void board_init(void)
{

  for(int i = 0; i<sizeof(pins); i++) {
    setup_pin(pins[i]);
  }

  xTaskCreate(&led_task, "led_task", 1024, NULL, 5, NULL);
}
