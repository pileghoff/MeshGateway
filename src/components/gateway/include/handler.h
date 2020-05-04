#include <stdint.h>

void handle_mqtt_event(char* topic,int topic_len, uint8_t* data, int data_len);
void handler_init();
