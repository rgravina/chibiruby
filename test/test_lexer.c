#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "lexer.h"

int tests_run = 0;
char* code = "10.times do |n|\n  puts n\nend";
/*
 * Result of running the above code in Ruby's Ripper.lex
 *
pp Ripper.lex(code)
[[[1, 0], :on_int, "10"],
 [[1, 2], :on_period, "."],
 [[1, 3], :on_ident, "times"],
 [[1, 8], :on_sp, " "],
 [[1, 9], :on_kw, "do"],
 [[1, 11], :on_sp, " "],
 [[1, 12], :on_op, "|"],
 [[1, 13], :on_ident, "n"],
 [[1, 14], :on_op, "|"],
 [[1, 15], :on_ignored_nl, "\n"],
 [[2, 0], :on_sp, "  "],
 [[2, 2], :on_ident, "puts"],
 [[2, 6], :on_sp, " "],
 [[2, 7], :on_ident, "n"],
 [[2, 8], :on_nl, "\n"],
 [[3, 0], :on_kw, "end"]]
 => [[[1, 0], :on_int, "10"], [[1, 2], :on_period, "."], [[1, 3], :on_ident, "times"], [[1, 8], :on_sp, " "], [[1, 9], :on_kw, "do"], [[1, 11], :on_sp, " "], [[1, 12], :on_op, "|"], [[1, 13], :on_ident, "n"], [[1, 14], :on_op, "|"], [[1, 15], :on_ignored_nl, "\n"], [[2, 0], :on_sp, "  "], [[2, 2], :on_ident, "puts"], [[2, 6], :on_sp, " "], [[2, 7], :on_ident, "n"], [[2, 8], :on_nl, "\n"], [[3, 0], :on_kw, "end"]]
*/
static char *test_lexer() {
  crb_lexer_lex(code);
  mu_assert("token line number incorrect", lexer->tokens[0]->lineno == 1);
  mu_assert("token start incorrect", lexer->tokens[0]->start == 0);
  mu_assert("token not parsed correctly", strcmp(lexer->tokens[0]->token->value, "10") == 0);
  return 0;
}

static char *all_tests() {
  mu_run_test(test_lexer);
  return 0;
}

int main() {
  crb_init_lexer();
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  }
  else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);
  crb_free_lexer();

  return result != 0;
}
