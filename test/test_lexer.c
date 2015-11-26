#include <stdio.h>
#include <string.h>
#include "tap.h"
#include "lexer.h"

void check_token(int lineno, int start, Type type, char* value) {
  Token* token = crb_next_token();
  cmp_ok(token->lineno, "==", lineno);
  cmp_ok(token->start, "==", start);
  cmp_ok(token->type, "==", type);
  is(token->value, value);
}

void test_lexer_method_block() {
  char* code = "10.times do |n|\n  puts n\nend";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 16);
  ok(lexer->in_token == false);
  check_token(1, 0, INTEGER, "10");
  check_token(1, 2, PERIOD, ".");
  check_token(1, 3, IDENTIFIER, "times");
  check_token(1, 8, SPACE, " ");
  check_token(1, 9, KEYWORD, "do");
  check_token(1, 11, SPACE, " ");
  check_token(1, 12, BAR, "|");
  check_token(1, 13, IDENTIFIER, "n");
  check_token(1, 14, BAR, "|");
  check_token(1, 15, NEWLINE, "\n");
  check_token(2, 0, SPACE, "  ");
  check_token(2, 2, IDENTIFIER, "puts");
  check_token(2, 6, SPACE, " ");
  check_token(2, 7, IDENTIFIER, "n");
  check_token(2, 8, NEWLINE, "\n");
  check_token(3, 0, KEYWORD, "end");
  crb_free_lexer();
}

void test_lexer_array() {
  char* code = "array = [1,2,3]\n puts array[1]";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 19);
  ok(lexer->in_token == false);
  check_token(1, 0, IDENTIFIER, "array");
  check_token(1, 5, SPACE, " ");
  check_token(1, 6, EQUAL, "=");
  check_token(1, 7, SPACE, " ");
  check_token(1, 8, LBRACKET, "[");
  check_token(1, 9, INTEGER, "1");
  check_token(1, 10, COMMA, ",");
  check_token(1, 11, INTEGER, "2");
  check_token(1, 12, COMMA, ",");
  check_token(1, 13, INTEGER, "3");
  check_token(1, 14, RBRACKET, "]");
  check_token(1, 15, NEWLINE, "\n");
  check_token(2, 0, SPACE, " ");
  check_token(2, 1, IDENTIFIER, "puts");
  check_token(2, 5, SPACE, " ");
  check_token(2, 6, IDENTIFIER, "array");
  check_token(2, 11, LBRACKET, "[");
  check_token(2, 12, INTEGER, "1");
  check_token(2, 13, RBRACKET, "]");
  crb_free_lexer();
}

void test_lexer_string_double_quote() {
  char* code = "puts \"Hello, World!\"";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 5);
  ok(lexer->in_token == false);
  check_token(1, 0, IDENTIFIER, "puts");
  check_token(1, 4, SPACE, " ");
  check_token(1, 5, STRING_BEGINING, "\"");
  check_token(1, 6, STRING_CONTENT, "Hello, World!");
  check_token(1, 19, STRING_END, "\"");
  crb_free_lexer();
}

void test_lexer_string_single_quote() {
  char* code = "puts 'Hello, World!'";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 5);
  ok(lexer->in_token == false);
  check_token(1, 0, IDENTIFIER, "puts");
  check_token(1, 4, SPACE, " ");
  check_token(1, 5, STRING_BEGINING, "'");
  check_token(1, 6, STRING_CONTENT, "Hello, World!");
  check_token(1, 19, STRING_END, "'");
  crb_free_lexer();
}

void test_lexer_colon() {
  char* code = ":hello\n::hello\n ::hello";
  crb_init_lexer(code);
  crb_lexer_lex();
  cmp_ok(lexer->num_tokens, "==", 9);
  ok(lexer->in_token == false);
  check_token(1, 0, SYMBOL_BEGINING, ":");
  check_token(1, 1, IDENTIFIER, "hello");
  check_token(1, 6, NEWLINE, "\n");
  check_token(2, 0, COLON2, "::");
  check_token(2, 2, IDENTIFIER, "hello");
  check_token(2, 7, NEWLINE, "\n");
  check_token(3, 0, SPACE, " ");
  check_token(3, 1, COLON3, "::");
  check_token(3, 3, IDENTIFIER, "hello");
  crb_free_lexer();
}

void test_lexer_brace() {
  char* code = "{1}";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 3);
  ok(lexer->in_token == false);
  check_token(1, 0, LBRACE, "{");
  check_token(1, 1, INTEGER, "1");
  check_token(1, 2, RBRACE, "}");
  crb_free_lexer();
}

void test_lexer_eof() {
  char* code = "1\0 2";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 1);
  ok(lexer->in_token == false);
  check_token(1, 0, INTEGER, "1");
  crb_free_lexer();
}

void test_lexer_exclamation() {
  char* code = "!";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 1);
  check_token(1, 0, NOT, "!");
  crb_free_lexer();

  code = "!=";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 1);
  check_token(1, 0, NOT_EQUAL, "!=");
  crb_free_lexer();

  code = "!~";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 1);
  check_token(1, 0, NOT_MATCH, "!~");
  crb_free_lexer();
}

void test_lexer_greater_than() {
  char* code = "1>2\n1>>2\n1>>=2\n1>=2\n";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 16);
  ok(lexer->in_token == false);
  check_token(1, 0, INTEGER, "1");
  check_token(1, 1, GREATER_THAN, ">");
  check_token(1, 2, INTEGER, "2");
  check_token(1, 3, NEWLINE, "\n");
  check_token(2, 0, INTEGER, "1");
  check_token(2, 1, RIGHT_SHIFT, ">>");
  check_token(2, 3, INTEGER, "2");
  check_token(2, 4, NEWLINE, "\n");
  check_token(3, 0, INTEGER, "1");
  check_token(3, 1, OP_ASSIGN, ">>=");
  check_token(3, 4, INTEGER, "2");
  check_token(3, 5, NEWLINE, "\n");
  check_token(4, 0, INTEGER, "1");
  check_token(4, 1, GREATER_THAN_OR_EQUAL, ">=");
  check_token(4, 3, INTEGER, "2");
  check_token(4, 4, NEWLINE, "\n");
  crb_free_lexer();
}

// TODO: more cases
/*
code = "`cvs diff parse.y`"
> pp Ripper.lex(code)
[[[1, 0], :on_backtick, "`"],
 [[1, 1], :on_tstring_content, "cvs diff parse.y"],
 [[1, 17], :on_tstring_end, "`"]]

code = 'obj.`("cvs diff parse.y")'
> pp Ripper.lex(code)
[[[1, 0], :on_ident, "obj"],
 [[1, 3], :on_period, "."],
 [[1, 4], :on_backtick, "`"],
 [[1, 5], :on_lparen, "("],
 [[1, 6], :on_tstring_beg, "\""],
 [[1, 7], :on_tstring_content, "cvs diff parse.y"],
 [[1, 23], :on_tstring_end, "\""],
 [[1, 24], :on_rparen, ")"]]

code = "print(<<EOS)"
> pp Ripper.lex(code)
[[[1, 0], :on_ident, "print"],
[[1, 5], :on_lparen, "("],
[[1, 6], :on_heredoc_beg, "<<EOS"]]

code = "class Greeter\n   def initialize(name = \"World\")\n     @name = name\n   end\n\n   def say_hi\n     puts \"Hi #{@name}!\"\n   end\n   def say_bye\n     puts \"Bye #{@name}, come back soon.\"\n   end\n end"
> pp Ripper.lex(code)
[[[1, 0], :on_kw, "class"],
 [[1, 5], :on_sp, " "],
 [[1, 6], :on_const, "Greeter"],
 [[1, 13], :on_nl, "\n"],
 [[2, 0], :on_sp, "   "],
 [[2, 3], :on_kw, "def"],
 [[2, 6], :on_sp, " "],
 [[2, 7], :on_ident, "initialize"],
 [[2, 17], :on_lparen, "("],
 [[2, 18], :on_ident, "name"],
 [[2, 22], :on_sp, " "],
 [[2, 23], :on_op, "="],
 [[2, 24], :on_sp, " "],
 [[2, 25], :on_tstring_beg, "\""],
 [[2, 26], :on_tstring_content, "World"],
 [[2, 31], :on_tstring_end, "\""],
 [[2, 32], :on_rparen, ")"],
 [[2, 33], :on_ignored_nl, "\n"],
 [[3, 0], :on_sp, "     "],
 [[3, 5], :on_ivar, "@name"],
 [[3, 10], :on_sp, " "],
 [[3, 11], :on_op, "="],
 [[3, 12], :on_sp, " "],
 [[3, 13], :on_ident, "name"],
 [[3, 17], :on_nl, "\n"],
 [[4, 0], :on_sp, "   "],
 [[4, 3], :on_kw, "end"],
 [[4, 6], :on_nl, "\n"],
 [[5, 0], :on_ignored_nl, "\n"],
 [[6, 0], :on_sp, "   "],
 [[6, 3], :on_kw, "def"],
 [[6, 6], :on_sp, " "],
 [[6, 7], :on_ident, "say_hi"],
 [[6, 13], :on_nl, "\n"],
 [[7, 0], :on_sp, "     "],
 [[7, 5], :on_ident, "puts"],
 [[7, 9], :on_sp, " "],
 [[7, 10], :on_tstring_beg, "\""],
 [[7, 11], :on_tstring_content, "Hi !"],
 [[7, 15], :on_tstring_end, "\""],
 [[7, 16], :on_nl, "\n"],
 [[8, 0], :on_sp, "   "],
 [[8, 3], :on_kw, "end"],
 [[8, 6], :on_nl, "\n"],
 [[9, 0], :on_sp, "   "],
 [[9, 3], :on_kw, "def"],
 [[9, 6], :on_sp, " "],
 [[9, 7], :on_ident, "say_bye"],
 [[9, 14], :on_nl, "\n"],
 [[10, 0], :on_sp, "     "],
 [[10, 5], :on_ident, "puts"],
 [[10, 9], :on_sp, " "],
 [[10, 10], :on_tstring_beg, "\""],
 [[10, 11], :on_tstring_content, "Bye , come back soon."],
 [[10, 32], :on_tstring_end, "\""],
 [[10, 33], :on_nl, "\n"],
 [[11, 0], :on_sp, "   "],
 [[11, 3], :on_kw, "end"],
 [[11, 6], :on_nl, "\n"],
 [[12, 0], :on_sp, " "],
 [[12, 1], :on_kw, "end"]]
 */

int main() {
  plan(NO_PLAN);
  test_lexer_method_block();
  test_lexer_array();
  test_lexer_string_double_quote();
  test_lexer_string_single_quote();
  test_lexer_colon();
  test_lexer_brace();
  test_lexer_eof();
  test_lexer_exclamation();
  test_lexer_greater_than();
  done_testing();
}
