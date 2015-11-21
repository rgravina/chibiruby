#include <stdio.h>
#include "minunit.h"
#include "lexer.h"

int tests_run = 0;
char* code = "10.times do |n|\n  puts n\nend";

static char *test_lexer() {
  mu_assert("error, string not retured", crb_lexer_lex(code) == code);
  return 0;
}

static char *all_tests() {
  mu_run_test(test_lexer);
  return 0;
}

int main() {
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  }
  else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);

  return result != 0;
}
