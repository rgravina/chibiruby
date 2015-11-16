#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(Lexer) {
  RUN_TEST_CASE(Lexer, lex_ShouldReturnOne);
}