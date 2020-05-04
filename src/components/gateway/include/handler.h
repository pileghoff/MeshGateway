#include <stdint.h>

/**
 * Handle incoming MQTT messages.
 * This ties the MQTT and BLE Mesh together.
 */ 
void handle_mqtt_event(char* topic,int topic_len, uint8_t* data, int data_len);

/**
 * Subscribe to the MQTT topics and setup the MQTT message handler
 */
void handler_init();
