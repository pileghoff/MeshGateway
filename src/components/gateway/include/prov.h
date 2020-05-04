#ifndef M_PROV
#define M_PROV
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

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
void prov_cb(esp_ble_mesh_prov_cb_event_t event,
                          esp_ble_mesh_prov_cb_param_t *param);

esp_ble_mesh_node_info_t *esp_ble_mesh_get_node_info(uint16_t unicast);

esp_err_t esp_ble_mesh_store_node_info(const uint8_t uuid[16], uint16_t unicast,
                                       uint8_t elem_num, uint8_t onoff_state);

void prov_init();

typedef struct {
  uint8_t uuid[16];
  uint8_t oob[6];
} device_t;

void start_prov(device_t* dev);

void start_reset(uint8_t uuid[16]);


#endif
