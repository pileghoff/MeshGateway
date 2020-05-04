#include "config.h"
#include "prov.h"
#include "common.h"
#include "network.h"
#include "mqtt.h"
#include "handler.h"
#include "mesh.h"
#include "board.h"


void app_main(void)
{

  board_init();
  mesh_init();
  network_init();
  board_boot();
  mqtt_init();
  handler_init();
}
