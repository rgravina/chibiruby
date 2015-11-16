#include "Lexer.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(Lexer);

TEST_SETUP(Lexer) {
}

TEST_TEAR_DOWN(Lexer) {
}

TEST(Lexer, lex_ShouldReturnOne) {
  TEST_ASSERT_EQUAL_INT(1, Lexer_lex());
}
