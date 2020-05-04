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

/**
 * Provisioning client callback function.
 */
void prov_cb(esp_ble_mesh_prov_cb_event_t event,
                          esp_ble_mesh_prov_cb_param_t *param);


/**
 * Initialize the required resources for the provisioning client
 */
void prov_init();

/**
 * This struct encapsulates a device to provision
 */
typedef struct {
  uint8_t uuid[16];
  uint8_t oob[6];
} device_t;

/**
 * Start the provisioning process for a device.
 */
void start_prov(device_t* dev);


#endif
