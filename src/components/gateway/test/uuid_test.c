#include <limits.h>
#include "unity.h"
#include "uuid_list.h"


TEST_CASE("uuid_add", "[uuid]")
{
  unprov_dev_t dev;
  TEST_ASSERT_EQUAL(0, uuid_add(&dev));
}
