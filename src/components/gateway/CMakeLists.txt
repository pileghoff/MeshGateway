idf_component_register(SRCS "board.c" "mesh.c" "config.c" "handler.c" "mqtt.c" "network.c" "prov.c" "uuid_list.c"
  INCLUDE_DIRS "./include"
  REQUIRES bt nvs_flash mqtt esp_eth)
