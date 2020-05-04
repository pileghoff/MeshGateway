#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "esp_ble_mesh_defs.h"

typedef esp_ble_mesh_unprov_dev_add_t unprov_dev_t;
// Returns 0 if item was added
// Returns 1 if the item was already present
// Returns -1 if the list was full
int uuid_add(unprov_dev_t* dev);


// Returns 0 if item is present, otherwise 1
unprov_dev_t* uuid_find(uint8_t uuid[16]);

// Returns 0 if item was removed, otherwise 1 (in case it was not there)
int uuid_remove(uint8_t uuid[16]);

int uuid_is_reset(uint8_t uuid[16]);

int uuid_reset(uint8_t uuid[16]);
