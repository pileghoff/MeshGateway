#ifndef MQTT_H
#define MQTT_H

void mqtt_send(char* topic, void* data, int len);
void mqtt_sub(const char* topic);
void mqtt_sub_done();
void mqtt_init();
void mqtt_log(const char* message, int level);

#endif
