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

#include "prov.h"
#include "config.h"
#include "common.h"
#include "uuid_list.h"
#include "mqtt.h"

#define PROV_OWN_ADDR       0x0001

#define TAG "mesh_prov"

extern uint8_t self_uuid[16];

uint8_t oob[6] = {0};

esp_ble_mesh_prov_t provision = {
  .prov_uuid           = self_uuid,
  .prov_unicast_addr   = PROV_OWN_ADDR,
  .prov_start_address  = 0x0005,
  .prov_attention      = 0x00,
  .prov_algorithm      = 0x00,
  .prov_pub_key_oob    = 0x00,
  .prov_static_oob_val = oob,
  .prov_static_oob_len = sizeof(oob),
  .flags               = 0x00,
  .iv_index            = 0x00,
};

QueueHandle_t prov_queue;
SemaphoreHandle_t prov_sem;
uint8_t current_uuid[16];

extern struct esp_ble_mesh_key {
  uint16_t net_idx;
  uint16_t app_idx;
  uint8_t  app_key[16];
} prov_key;


void start_prov(device_t* dev) {
  ESP_LOGI(TAG, "Prov started");
  mqtt_log("Provisioning started", 0);
  if(!xQueueSend(prov_queue, dev, 100)) {
    ESP_LOGW(TAG, "Prov failed to start");
    mqtt_log("Prov failed to start", 2);
  }
}

static void prov_task() {
  device_t dev;
  ESP_LOGI(TAG, "Prov task started");

  while(true) {
    if(xQueueReceive(prov_queue, &dev, 100)) {
      ESP_LOGI(TAG, "Prov task running");
      unprov_dev_t* unprov_dev = uuid_find(dev.uuid);

      if(unprov_dev != NULL) {
        uuid_remove(dev.uuid);
        while(xSemaphoreTake(prov_sem,100) == false) {
          vTaskDelay(100);
          ESP_LOGI(TAG, "Wait to prov: %s", bt_hex(dev.uuid, 16));
        }

        vTaskDelay(100);
        memcpy(oob, dev.oob, 6);
        ESP_LOGI(TAG, "OOB: %s", bt_hex(dev.oob, 6));
        memcpy(current_uuid, &dev.uuid, 16);
        ESP_LOGI(TAG, "Added unprovisioned device to queue %s", bt_hex(dev.uuid, 16));
        esp_ble_mesh_provisioner_add_unprov_dev(unprov_dev,
                                                ADD_DEV_RM_AFTER_PROV_FLAG | ADD_DEV_FLUSHABLE_DEV_FLAG);
      }
    }
  }
}

void end_prov(int reason) {
  ESP_LOGI(TAG, "Prov ended with reason %d", reason);
  if(reason != 0) {
    ESP_LOGI(TAG, "Removed device from queue %s", bt_hex(current_uuid, 16));
    esp_ble_mesh_device_delete_t dev;
    dev.flag = BIT(1);
    memcpy(dev.uuid, current_uuid, 16);
    if(esp_ble_mesh_provisioner_delete_dev(&dev) != ESP_OK) {
      ESP_LOGE(TAG, "Failed to remove device from queue");
    }
    vTaskDelay(10);

    mqtt_send("/device/prov/callback", &reason, 1);
  }

  xSemaphoreGive(prov_sem);
}


esp_err_t prov_complete(int node_idx, const esp_ble_mesh_octet16_t uuid,
                               uint16_t unicast, uint8_t elem_num, uint16_t net_idx)
{
    char name[10];
    int err;

    ESP_LOGI(TAG, "node index: 0x%x, unicast address: 0x%02x, element num: %d, netkey index: 0x%02x",
             node_idx, unicast, elem_num, net_idx);
    ESP_LOGI(TAG, "device uuid: %s", bt_hex(uuid, 16));

    sprintf(name, "%s%d", "NODE-", node_idx);
    err = esp_ble_mesh_provisioner_set_node_name(node_idx, name);
    if (err) {
        ESP_LOGE(TAG, "%s: Set node name failed", __func__);
        return ESP_FAIL;
    }

    uint8_t buf[1+2];
    buf[0] = 0;
    memcpy(&buf[1], &unicast, sizeof(uint16_t));
    mqtt_send("/device/prov/callback", buf, 3);

    return ESP_OK;
}

void new_device(uint8_t dev_uuid[16], uint8_t addr[ESP_BD_ADDR_LEN],
                                esp_ble_addr_type_t addr_type, uint16_t oob_info,
                                uint8_t adv_type, esp_ble_mesh_prov_bearer_t bearer)
{

  if(bearer == ESP_BLE_MESH_PROV_ADV) {
    unprov_dev_t add_dev = {0};

    memcpy(add_dev.addr, addr, ESP_BD_ADDR_LEN);
    add_dev.addr_type = (uint8_t)addr_type;
    memcpy(add_dev.uuid, dev_uuid, 16);
    add_dev.oob_info = oob_info;
    add_dev.bearer = (uint8_t)bearer;

    mqtt_send("/device/new", dev_uuid, 16);
    uuid_add(&add_dev);
  }
}

void prov_cb(esp_ble_mesh_prov_cb_event_t event,
                                 esp_ble_mesh_prov_cb_param_t *param)
{
    switch (event) {
    case ESP_BLE_MESH_PROVISIONER_RECV_UNPROV_ADV_PKT_EVT:
        new_device(param->provisioner_recv_unprov_adv_pkt.dev_uuid, param->provisioner_recv_unprov_adv_pkt.addr,
                            param->provisioner_recv_unprov_adv_pkt.addr_type, param->provisioner_recv_unprov_adv_pkt.oob_info,
                            param->provisioner_recv_unprov_adv_pkt.adv_type, param->provisioner_recv_unprov_adv_pkt.bearer);
        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_OPEN_EVT:
      ESP_LOGI(TAG, "Link open");
      break;
    case ESP_BLE_MESH_PROVISIONER_PROV_LINK_CLOSE_EVT:
        end_prov(param->provisioner_prov_link_close.reason);

        break;
    case ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT:
        prov_complete(param->provisioner_prov_complete.node_idx, param->provisioner_prov_complete.device_uuid,
                      param->provisioner_prov_complete.unicast_addr, param->provisioner_prov_complete.element_num,
                      param->provisioner_prov_complete.netkey_idx);
        break;
    case ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT: {
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_SET_NODE_NAME_COMP_EVT, err_code %d", param->provisioner_set_node_name_comp.err_code);
        if (param->provisioner_set_node_name_comp.err_code == ESP_OK) {
            const char *name = NULL;
            name = esp_ble_mesh_provisioner_get_node_name(param->provisioner_set_node_name_comp.node_index);
            if (!name) {
                ESP_LOGE(TAG, "Get node name failed");
                return;
            }
            ESP_LOGI(TAG, "Node %d name is: %s", param->provisioner_set_node_name_comp.node_index, name);
        }
        break;
    }
    case ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT: {
        ESP_LOGI(TAG, "ESP_BLE_MESH_PROVISIONER_ADD_LOCAL_APP_KEY_COMP_EVT, err_code %d", param->provisioner_add_app_key_comp.err_code);
        if (param->provisioner_add_app_key_comp.err_code == ESP_OK) {
            prov_key.app_idx = param->provisioner_add_app_key_comp.app_idx;
        }
        break;
    }
    default:
      ESP_LOGW(TAG, "Unhandled prov event %d", event);
        break;
    }

    return;
}


void prov_init() {
  prov_sem = xSemaphoreCreateBinary();
  xSemaphoreGive(prov_sem);

  prov_queue = xQueueCreate(10, sizeof(device_t));
  assert(prov_queue != 0);

  xTaskCreate(&prov_task, "prov_task", 2048, NULL, 5, NULL);
}
