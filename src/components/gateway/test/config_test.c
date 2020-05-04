#include <string.h>
#include <stdint.h>

#include "config.h"
#include "config_mocks.h"
#include "unity.h"


TEST_CASE("config_reset", "[config]")
{
  
  config_reset(0x0);
  TEST_ASSERT_EQUAL(1, xQueueGenericSend_fake.call_count);

}
