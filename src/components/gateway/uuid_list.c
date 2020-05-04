#include "uuid_list.h"

#define NUM_UUID 10

static uint8_t uuid_list[NUM_UUID][16];
static uint8_t uuid_present[NUM_UUID];

unprov_dev_t dev_list[NUM_UUID];


// Returns 0 if item was added
// Returns 1 if the item was already present
// Returns -1 if the list was full
int uuid_add(unprov_dev_t* dev) {
  uint8_t *uuid = dev->uuid;

  if(uuid_find(uuid) != NULL) {
    return 1;
  }

  for(int i = 0; i < NUM_UUID; i++) {
    if(uuid_present[i] == 0) {
      memcpy(uuid_list[i], uuid, 16);
      memcpy(&dev_list[i], dev, sizeof(unprov_dev_t));
      uuid_present[i] = 1;
      return 0;
    }
  }

  return -1;
}

// Returns 0 if item is present, otherwise 1
unprov_dev_t* uuid_find(uint8_t uuid[16]) {
  for(int i = 0; i < NUM_UUID; i++) {
    if(uuid_present[i] == 1 && memcmp(uuid, uuid_list[i], 16) == 0) {
      return &dev_list[i];
    }
  }

  return NULL;
}

// Returns 0 if item was removed, otherwise 1 (in case it was not there)
int uuid_remove(uint8_t uuid[16]) {
  if(uuid_find(uuid) == NULL) {
    return 1;
  }

  for(int i = 0; i < NUM_UUID; i++) {
    if(memcmp(uuid, uuid_list[i], 16) == 0) {
      uuid_present[i] = 0;
      return 0;
    }
  }

  return -1;
}
