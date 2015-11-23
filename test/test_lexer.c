#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "lexer.h"

int tests_run = 0;
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
  char* code = "10.times do |n|\n  puts n\nend";
  crb_init_lexer();
  crb_lexer_lex(code);

  mu_assert(lexer->head->lineno == 1, "token line number incorrect");
  mu_assert(lexer->head->start == 0, "token start incorrect");
  mu_assert(strcmp(lexer->head->value, "10") == 0, "token not parsed correctly");
  mu_assert(lexer->head->type == INTEGER, "token type not set correctly");
  mu_assert(lexer->num_tokens == 2, "token count incorrect");

  mu_assert(lexer->head->next->type == PERIOD, "token type not set correctly");
  mu_assert(lexer->in_token == false, "in_token flag not set to false after processing token");
  crb_free_lexer();
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
