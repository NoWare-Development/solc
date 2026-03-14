#include "solc/init.h"
#include "global.h"
#include "parser/parser_context.h"

void solc_init(void)
{
  global_init();

  parser_context_initialize();
}

void solc_deinit(void)
{
  global_deinit();
}
