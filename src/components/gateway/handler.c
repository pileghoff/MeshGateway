#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "prov.h"
#include "config.h"
#include "mqtt.h"
#include "board.h"

#define TOPIC_NUM (sizeof(topics)/sizeof(topics[0]))

static const char* TAG ="HANDLER";
const char topics[][22] = {"/device/prov",
                           "/device/conf/sub/add",
                           "/device/conf/sub/del",
                           "/device/conf/pub",
                           "/device/conf/key/bind",
                           "/device/conf/reset",
                           "/device/conf/comp/get",
                           "/device/conf/key/add",
                           "/device/error"};


const int topic_data_size[] = { sizeof(device_t),
                                3*sizeof(uint16_t),
                                2*sizeof(uint16_t),
                                3*sizeof(uint16_t),
                                2*sizeof(uint16_t),
                                sizeof(uint16_t),
                                sizeof(uint16_t),
                                sizeof(uint16_t),
                                sizeof(uint8_t)};

void handle_mqtt_event(char* topic, int topic_len, uint8_t* data, int data_len) {
  ESP_LOGI(TAG, "Handler %s len: %d", topic, data_len);
  int topic_num = -1;
  for(int i = 0; i < TOPIC_NUM; i++) {
    if(memcmp(topic, topics[i], topic_len*sizeof(char)) == 0) {
      if(data_len != topic_data_size[i]) {
        topic_num = -2;
        break;
      }
      topic_num = i;
      break;
    }
  }

  uint8_t ret = 0;
  uint16_t* d16 = (uint16_t*) data;
  switch(topic_num) {
  case 0:
    start_prov((device_t *) data);
    break;
  case 1:
    config_sub_add(d16[0], d16[1], d16[2]);
    break;
  case 2:
    config_sub_delete(d16[0], d16[1]);
    break;
  case 3:
    config_pub_set(d16[0], d16[1], d16[2]);
    break;
  case 4:
    config_model_app_bind(d16[0], d16[1]);
    break;
  case 5:
    config_reset(d16[0]);
    break;
  case 6:
    config_comp_data_get(d16[0]);
    break;
  case 7:
    config_app_key_add(d16[0]);
    break;
  case 8:
    board_err(data[0]);
    mqtt_send("/device/conf/callback", &ret, 1);
    break;
  case -1:
    ESP_LOGW(TAG, "Unhandled MQTT event");
    break;
  case -2:
    ESP_LOGW(TAG, "Size mismatch");
    break;
  }


}

void handler_init() {
  for(int i = 0; i < TOPIC_NUM; i++) {
    mqtt_sub(topics[i]);
    ESP_LOGI(TAG, "Subscribe to mqtt %s", topics[i]);
  }

  mqtt_sub_done();
}
