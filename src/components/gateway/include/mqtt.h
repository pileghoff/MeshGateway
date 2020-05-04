#ifndef MQTT_H
#define MQTT_H

/**
 *  Publish a MQTT message. If MQTT has not been started properly yet, sleep.
 */
void mqtt_send(char* topic, void* data, int len);


/**
 *  Subscribe to a MQTT topic. If MQTT has not been started properly yet, sleep.
 */
void mqtt_sub(const char* topic);

/**
 * Signal to the system that all MQTT topics has been successfully subscribed to.
 */
void mqtt_sub_done();

void mqtt_init();

/**
 * Send a log message to the logging DB over MQTT.
 */
void mqtt_log(const char* message, int level);

#endif
