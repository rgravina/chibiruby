#include <stdio.h>
#include <string.h>
#include "tap.h"
#include "test_lexer.h"
#include "test_parser.h"

int main() {
  plan(NO_PLAN);
  crb_run_lexer_tests();
  crb_run_parser_tests();
  done_testing();
}
