#include "Lexer.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(Lexer);

/*
> pp Ripper.lex("10.times do |n|\n  puts n\nend")
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
*/
char *code;
TEST_SETUP(Lexer) {
  code = "10.times do |n|\n  puts n\nend";
}

TEST_TEAR_DOWN(Lexer) {
}

TEST(Lexer, lex_ShouldReturnString) {
  TEST_ASSERT_EQUAL_STRING(code, Lexer_lex(code));
}
