#include "solc/init.h"
#include "global.h"

void solc_init(void)
{
  global_init();
}

void solc_deinit(void)
{
  global_deinit();
}
