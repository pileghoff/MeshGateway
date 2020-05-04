#include <limits.h>
#include <string.h>
#include "unity.h"
#include "mqtt.h"
#include "mqtt_mocks.h"

extern volatile int connected;
extern volatile int ready;


TEST_CASE("mqtt_init", "[mqtt]")
{

  connected = true;
  mqtt_init();
  TEST_ASSERT_EQUAL(1, esp_mqtt_client_init_fake.call_count);
  TEST_ASSERT_EQUAL(1, esp_mqtt_client_start_fake.call_count);

}

TEST_CASE("mqtt_sub", "[mqtt]")
{

  connected = true;
  mqtt_sub_done();
  TEST_ASSERT_EQUAL(true, ready);

  mqtt_sub("");
  TEST_ASSERT_EQUAL(1, esp_mqtt_client_subscribe_fake.call_count);
}

TEST_CASE("mqtt_send", "[mqtt]")
{

  connected = true;
  mqtt_send("topic", "data", 5);
  TEST_ASSERT_EQUAL(1, esp_mqtt_client_publish_fake.call_count);
  TEST_ASSERT_EQUAL(0, strncmp(esp_mqtt_client_publish_fake.arg1_val, "topic", 6));
  TEST_ASSERT_EQUAL(0, strncmp(esp_mqtt_client_publish_fake.arg2_val, "data", 6));
  TEST_ASSERT_EQUAL(5, esp_mqtt_client_publish_fake.arg3_val);
}
