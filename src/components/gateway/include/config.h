#ifndef CONFIG_H
#define CONFIG_H


#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_common_api.h"
#include "esp_ble_mesh_provisioning_api.h"
#include "esp_ble_mesh_networking_api.h"
#include "esp_ble_mesh_config_model_api.h"
#include "esp_ble_mesh_generic_model_api.h"
#include "common.h"

extern esp_ble_mesh_client_t config_client;
extern esp_ble_mesh_client_common_param_t c_common;
extern esp_ble_mesh_client_t onoff_client;

void config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                                   esp_ble_mesh_cfg_client_cb_param_t *param);
void config_init();

void config_pub_set(uint16_t addr, uint16_t publish_addr, uint16_t model_id);

void config_sub_add(uint16_t addr, uint16_t sub_addr, uint16_t model_id);

void config_sub_delete(uint16_t addr, uint16_t model_id);

void config_comp_data_get(uint16_t addr);

void config_app_key_add(uint16_t addr);

void config_model_app_bind(uint16_t addr, uint16_t model_id);

void config_comp_data_get(uint16_t addr);

void config_app_key_add(uint16_t addr);

void config_reset(uint16_t addr);

esp_ble_mesh_client_common_param_t new_param();

#endif
