#ifndef _BOARD_H_
#define _BOARD_H_

#include "esp_ble_mesh_defs.h"

/**
 *  Set LED's to indicate bard has successfully booted
 */
void board_boot();

/**
 *  Make the network LED blink, as ethernet is up.
 *  When MQTT goes up aswell, the network LED is solid.
 */
void board_eth();

/**
 *  Make the network LED light up.
 */
void board_mqtt();

/**
 *  Indicate an error using the red LED on the board.
 */
void board_err(int err);

/**
 *  Initialize all GPIO's and start the task that handles
 *  LED blinking.
 */
void board_init();

#endif
