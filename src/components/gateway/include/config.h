/** \file
 *  This module handles the configuration client for BLE Mesh
 *
 *  It uses a queue where commands are added as they appear.
 *  This queue is then consumed by a single task, to make sure
 *  new commands are not send before the response to a previous has been received.
 */
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

/**
 * Callback function to handle configuration client events
 */
void config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                                   esp_ble_mesh_cfg_client_cb_param_t *param);

/**
 * Initialize the required resources for the configuration client.
 */
void config_init();

/**
 * Add a command to the queue to set the publication address of a device
 * \param addr Address of the device to configure
 * \param publish_addr Publish address to set for the model
 * \param model_id The model to configure
 */
void config_pub_set(uint16_t addr, uint16_t publish_addr, uint16_t model_id);

/**
 * Add a command to the queue to add a subscription address of a device
 * \param addr Address of the device to configure
 * \param sub_addr Subscription address to set for the model
 * \param model_id The model to configure
 */
void config_sub_add(uint16_t addr, uint16_t sub_addr, uint16_t model_id);

/**
 * Add a command to the queue to delete all subscriptions for a model
 * \param addr Address of the device to configure
 * \param model_id The model to configure
 */
void config_sub_delete(uint16_t addr, uint16_t model_id);


/**
 * Add a command to the queue to get the composition data
 * \param addr Address of the device to configure
 */
void config_comp_data_get(uint16_t addr);


/**
 * Add a command to the queue to add an application key to the device
 * \param addr Address of the device to configure
 */
void config_app_key_add(uint16_t addr);


/**
 * Add a command to the queue to bind an application key to a model
 * \param addr Address of the device to configure
 * \param model_id Model to bind the key to
 */
void config_model_app_bind(uint16_t addr, uint16_t model_id);

void config_reset(uint16_t addr);

esp_ble_mesh_client_common_param_t new_param();

#endif
