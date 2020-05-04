#ifndef _BOARD_H_
#define _BOARD_H_

#include "esp_ble_mesh_defs.h"

void board_boot();

void board_eth();

void board_mqtt();

void board_err(int err);

void board_init();

#endif
