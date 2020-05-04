#ifndef MQTT_MOCKS_H
#define MQTT_MOCKS_H

#include "mqtt_client.h"
#include "fff.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(esp_mqtt_client_handle_t, esp_mqtt_client_init, const esp_mqtt_client_config_t*);
FAKE_VALUE_FUNC(esp_err_t, esp_mqtt_client_start, esp_mqtt_client_handle_t);
FAKE_VALUE_FUNC(int, esp_mqtt_client_subscribe,esp_mqtt_client_handle_t, const char*, int);
FAKE_VALUE_FUNC(int, esp_mqtt_client_publish, esp_mqtt_client_handle_t, const char*, const char*, int, int, int);

#endif
