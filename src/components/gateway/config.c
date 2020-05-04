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

#include "config.h"
#include "mqtt.h"

#define TAG "mesh_config"

// Config opcodes
#define CONFIG_OP_SUB_ADD  0x1
#define CONFIG_OP_PUB_SET  0x2
#define CONFIG_OP_COMP_GET 0x3
#define CONFIG_OP_APP_BIND 0x4
#define CONFIG_OP_KEY_ADD  0x5
#define CONFIG_OP_SUB_DEL  0x6
#define CONFIG_OP_RESET    0x7

#define MAX_CONFIG_QUEUE 7

esp_ble_mesh_client_t config_client;

SemaphoreHandle_t config_sem;
QueueHandle_t config_queue;

extern struct esp_ble_mesh_key {
   uint16_t net_idx;
   uint16_t app_idx;
   uint8_t  app_key[16];
} prov_key;

void config_reset(uint16_t addr)
{
   uint8_t buf[MAX_CONFIG_QUEUE] = {0};
   uint16_t *buf16 = (uint16_t*)(&buf[1]);

   buf[0] = CONFIG_OP_RESET;
   buf16[0] = addr;
   xQueueSend(config_queue, (void * ) &buf, 0);
}

esp_err_t _config_reset(uint16_t addr) {
   ESP_LOGI(TAG, "Reset node: %#06x", addr);
   esp_ble_mesh_client_common_param_t c = new_param();
   c.opcode = ESP_BLE_MESH_MODEL_OP_NODE_RESET;
   c.model = config_client.model;
   c.ctx.addr = addr;

   esp_ble_mesh_cfg_client_set_state_t set_state = {0};

   int err = esp_ble_mesh_config_client_set_state(&c, &set_state);
   if (err) {
      ESP_LOGE(TAG, "%s: Config publish set failed", __func__);
   }
   return err;
}

void config_sub_add(uint16_t addr, uint16_t sub_addr, uint16_t model_id) {
   uint8_t buf[MAX_CONFIG_QUEUE] = {0};
   uint16_t *buf16 = (uint16_t*)(&buf[1]);

   buf[0] = CONFIG_OP_SUB_ADD;
   buf16[0] = addr;
   buf16[1] = sub_addr;
   buf16[2] = model_id;
   xQueueSend(config_queue, (void * ) &buf, 0);
}

esp_err_t _config_sub_add(uint16_t addr, uint16_t sub_addr, uint16_t model_id) {
   ESP_LOGI(TAG, "Sub add: %#06x %#06x %#06x", addr, sub_addr, model_id);
   esp_ble_mesh_cfg_model_sub_add_t sub_set = {0};


   esp_ble_mesh_client_common_param_t c = new_param();
   c.opcode = ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD;
   c.model = config_client.model;
   c.ctx.addr = addr;

   sub_set.element_addr = addr;
   sub_set.sub_addr = sub_addr;
   sub_set.model_id = model_id;
   sub_set.company_id = 0xFFFF;

   esp_ble_mesh_cfg_client_set_state_t set_state = {0};
   set_state.model_sub_add = sub_set;


   int err = esp_ble_mesh_config_client_set_state(&c, &set_state);
   if (err) {
      ESP_LOGE(TAG, "%s: Config publish set failed", __func__);
   }
   return err;
}

void config_sub_delete(uint16_t addr, uint16_t model_id) {
   uint8_t buf[MAX_CONFIG_QUEUE] = {0};
   uint16_t *buf16 = (uint16_t*)(&buf[1]);

   buf[0] = CONFIG_OP_SUB_DEL;
   buf16[0] = addr;
   buf16[1] = model_id;
   buf16[2] = 0;
   xQueueSend(config_queue, (void * ) &buf, 0);
}

esp_err_t _config_sub_delete(uint16_t addr, uint16_t model_id) {


   ESP_LOGI(TAG, "Sub delete: %#06x %#06x", addr, model_id);
   esp_ble_mesh_cfg_model_sub_delete_all_t sub_set = {0};

   esp_ble_mesh_client_common_param_t c = new_param();
   c.opcode = ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL;
   c.model = config_client.model;
   c.ctx.addr = addr;

   sub_set.element_addr = addr;
   sub_set.model_id = model_id;
   sub_set.company_id = 0xFFFF;

   esp_ble_mesh_cfg_client_set_state_t set_state = {0};
   set_state.model_sub_delete_all = sub_set;


   int err = esp_ble_mesh_config_client_set_state(&c, &set_state);
   if (err) {
      ESP_LOGE(TAG, "%s: Config publish set failed", __func__);
   }
   return err;
}

void config_pub_set(uint16_t addr, uint16_t publish_addr, uint16_t model_id) {
   uint8_t buf[MAX_CONFIG_QUEUE] = {0};
   uint16_t *buf16 = (uint16_t*)(&buf[1]);

   buf[0] = CONFIG_OP_PUB_SET;
   buf16[0] = addr;
   buf16[1] = publish_addr;
   buf16[2] = model_id;
   xQueueSend(config_queue, (void * ) &buf, 0);
}

esp_err_t _config_pub_set(uint16_t addr, uint16_t publish_addr, uint16_t model_id) {

   ESP_LOGI(TAG, "Pub set: %#06x %#06x %#06x", addr, publish_addr, model_id);

   esp_ble_mesh_cfg_model_pub_set_t pub_set = {0};

   esp_ble_mesh_client_common_param_t c = new_param();
   c.opcode = ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET;
   c.model = config_client.model;
   c.ctx.addr = addr;

   pub_set.element_addr = addr;
   pub_set.publish_addr = publish_addr;
   pub_set.publish_app_idx = prov_key.app_idx;
   pub_set.cred_flag = false;
   pub_set.publish_ttl = 0xFF;
   pub_set.publish_period = 0;
   pub_set.publish_retransmit = 1;
   pub_set.model_id = model_id;
   pub_set.company_id = 0xFFFF;

   esp_ble_mesh_cfg_client_set_state_t set_state = {0};
   set_state.model_pub_set = pub_set;


   int err = esp_ble_mesh_config_client_set_state(&c, &set_state);
   if (err) {
      ESP_LOGE(TAG, "%s: Config publish set failed", __func__);
   }
   return err;
}

void config_comp_data_get(uint16_t addr) {
   uint8_t buf[MAX_CONFIG_QUEUE] = {0};
   uint16_t *buf16 = (uint16_t*)(&buf[1]);

   buf[0] = CONFIG_OP_COMP_GET;
   buf16[0] = addr;
   buf16[1] = 0;
   buf16[2] = 0;
   xQueueSend(config_queue, (void * ) &buf, 0);
}

esp_err_t _config_comp_data_get(uint16_t addr) {

   esp_ble_mesh_cfg_client_get_state_t data = {0};
   esp_ble_mesh_client_common_param_t c = new_param();
   c.opcode = ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET;
   c.model = config_client.model;
   c.ctx.addr = addr;

   data.comp_data_get.page = COMP_DATA_PAGE_0;

   int err = esp_ble_mesh_config_client_get_state(&c, &data);
   if (err) {
      ESP_LOGE(TAG, "Send config comp data get failed");
   }
   return err;
}


void config_app_key_add(uint16_t addr) {
   uint8_t buf[MAX_CONFIG_QUEUE] = {0};
   uint16_t *buf16 = (uint16_t*)(&buf[1]);

   buf[0] = CONFIG_OP_KEY_ADD;
   buf16[0] = addr;
   buf16[1] = 0;
   buf16[2] = 0;
   xQueueSend(config_queue, (void * ) &buf, 0);
}

esp_err_t _config_app_key_add(uint16_t addr) {

   esp_ble_mesh_cfg_client_set_state_t set_state = {0};

   esp_ble_mesh_client_common_param_t c = new_param();
   c.opcode = ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD;
   c.model = config_client.model;
   c.ctx.addr = addr;

   set_state.app_key_add.net_idx = prov_key.net_idx;
   set_state.app_key_add.app_idx = prov_key.app_idx;

   memcpy(set_state.app_key_add.app_key, prov_key.app_key, 16);

   int err = esp_ble_mesh_config_client_set_state(&c, &set_state);
   if (err) {
      ESP_LOGE(TAG, "%s: Config AppKey Add failed", __func__);
   }

   return err;
}

void config_model_app_bind(uint16_t addr, uint16_t model_id) {
   uint8_t buf[MAX_CONFIG_QUEUE] = {0};
   uint16_t *buf16 = (uint16_t*)(&buf[1]);

   buf[0] = CONFIG_OP_APP_BIND;
   buf16[0] = addr;
   buf16[1] = model_id;
   buf16[2] = 0;
   xQueueSend(config_queue, (void * ) &buf, 0);
}

esp_err_t _config_model_app_bind(uint16_t addr, uint16_t model_id) {


   esp_ble_mesh_cfg_client_set_state_t set_state = {0};

   esp_ble_mesh_client_common_param_t c = new_param();
   c.opcode = ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND;
   c.model = config_client.model;
   c.ctx.addr = addr;

   set_state.model_app_bind.element_addr = addr;
   set_state.model_app_bind.model_app_idx = prov_key.app_idx;
   set_state.model_app_bind.model_id = model_id;
   set_state.model_app_bind.company_id = CID_NVAL;
   int err = esp_ble_mesh_config_client_set_state(&c, &set_state);
   if (err) {
      ESP_LOGE(TAG, "%s: Config Model App Bind failed", __func__);
   }

   return err;
}

void config_task() {
   uint8_t buf[MAX_CONFIG_QUEUE];
   while(true) {
      if(xQueueReceive(config_queue, &buf, 500)) {
         while(xSemaphoreTake(config_sem,100) == false) {
            ESP_LOGI(TAG, "Sem waiting %d", buf[0]);
            vTaskDelay(100);
         }

         ESP_LOGI(TAG, "Sem taken %d", buf[0]);

         uint16_t *buf16 = (uint16_t*)(&buf[1]);
         ESP_LOGI(TAG, "Config task buf: %s", bt_hex(buf16, 3*sizeof(uint16_t)));

         switch(buf[0]) {
            case CONFIG_OP_SUB_ADD:
               _config_sub_add(buf16[0], buf16[1], buf16[2]);
               break;

            case CONFIG_OP_SUB_DEL:
               _config_sub_delete(buf16[0], buf16[1]);
               break;

            case CONFIG_OP_PUB_SET:
               _config_pub_set(buf16[0], buf16[1], buf16[2]);
               break;

            case CONFIG_OP_COMP_GET:
               _config_comp_data_get(buf16[0]);
               break;

            case CONFIG_OP_APP_BIND:
               _config_model_app_bind(buf16[0], buf16[1]);
               break;

            case CONFIG_OP_KEY_ADD:
               _config_app_key_add(buf16[0]);
               break;

            case CONFIG_OP_RESET:
               _config_reset(buf16[0]);
               break;
         }
      } else {
         ESP_LOGI(TAG, "Nothing in config queue");
      }
   }
}

void config_client_cb(esp_ble_mesh_cfg_client_cb_event_t event,
                      esp_ble_mesh_cfg_client_cb_param_t *param)
{
   uint32_t opcode;

   opcode = param->params->opcode;

   ESP_LOGI(TAG, "%s, error_code = 0x%02x, event = 0x%02x, addr: 0x%04x, opcode: 0x%04x",
            __func__, param->error_code, event, param->params->ctx.addr, opcode);



   xSemaphoreGive(config_sem);

   if (param->error_code) {
      xSemaphoreGive(config_sem);
      ESP_LOGW(TAG, "Sem given");

      ESP_LOGE(TAG, "Send config client message failed, opcode 0x%04x", opcode);

      uint8_t ret = -1;
      mqtt_send("/device/conf/callback", &ret, 1);
      return;
   }

   switch (event) {
      case ESP_BLE_MESH_CFG_CLIENT_GET_STATE_EVT:

         switch (opcode) {

            case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET: {
               ESP_LOGI(TAG, "composition data %s", bt_hex(param->status_cb.comp_data_status.composition_data->data,
                                                           param->status_cb.comp_data_status.composition_data->len));

               uint8_t ret = 0;
               mqtt_send("/device/conf/callback", &ret, 1);
               break;
            }
            default:
               break;
         }
         break;
      case ESP_BLE_MESH_CFG_CLIENT_SET_STATE_EVT:

         switch (opcode) {
         case ESP_BLE_MESH_MODEL_OP_NODE_RESET:
            case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
            case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
            case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET:
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL:
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD: {
              ESP_LOGW(TAG, "Sending ok");
               uint8_t ret = 0;
               mqtt_send("/device/conf/callback", &ret, 1);
               break;
            }
            default:
               break;
         }
         break;
      case ESP_BLE_MESH_CFG_CLIENT_PUBLISH_EVT:
         switch (opcode) {
            case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_STATUS:
               ESP_LOG_BUFFER_HEX("composition data %s", param->status_cb.comp_data_status.composition_data->data,
                                  param->status_cb.comp_data_status.composition_data->len);
               break;
            case ESP_BLE_MESH_MODEL_OP_APP_KEY_STATUS:
               break;
            default:
               break;
         }
         break;
      case ESP_BLE_MESH_CFG_CLIENT_TIMEOUT_EVT:

         ESP_LOGW(TAG, "Timeout detected");
         switch (opcode) {
            // Node reset allways returns a timeout
            case ESP_BLE_MESH_MODEL_OP_NODE_RESET:
            case ESP_BLE_MESH_MODEL_OP_APP_KEY_ADD:
            case ESP_BLE_MESH_MODEL_OP_COMPOSITION_DATA_GET:
            case ESP_BLE_MESH_MODEL_OP_MODEL_APP_BIND:
            case ESP_BLE_MESH_MODEL_OP_MODEL_PUB_SET:
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_DELETE_ALL:
            case ESP_BLE_MESH_MODEL_OP_MODEL_SUB_ADD: {
              ESP_LOGW(TAG, "Sending timeout");
               uint8_t ret = 1;
               mqtt_send("/device/conf/callback", &ret, 1);
               break;
            }
            default:
               ESP_LOGE(TAG, "Unknown opcode %d for timeout", opcode);
               break;
         }
         break;
      default:
         ESP_LOGE(TAG, "Not a config client status message event");
         break;
   }
}


esp_ble_mesh_client_common_param_t new_param() {
  esp_ble_mesh_client_common_param_t c;
  c.ctx.net_idx = prov_key.net_idx;
  c.ctx.app_idx = prov_key.app_idx;
  c.ctx.send_ttl = MSG_SEND_TTL;
  c.ctx.send_rel = MSG_SEND_REL;
  c.msg_timeout = MSG_TIMEOUT;
  c.msg_role = MSG_ROLE;
  return c;
}


void config_init() {

   config_sem = xSemaphoreCreateBinary();
   xSemaphoreGive(config_sem);

   config_queue = xQueueCreate(10, 3*sizeof(uint16_t)+sizeof(uint8_t));
   xTaskCreate(&config_task, "config_task", 2048, NULL, 5, NULL);
}
