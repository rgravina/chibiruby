#include <stdio.h>
#include <string.h>
#include "tap.h"
#include "lexer.h"

void check_token(int lineno, int start, TokenType type, char* value) {
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
  check_token(1, 0, tINTEGER, "10");
  check_token(1, 2, tPERIOD, ".");
  check_token(1, 3, tIDENTIFIER, "times");
  check_token(1, 8, tSPACE, " ");
  check_token(1, 9, tKEYWORD, "do");
  check_token(1, 11, tSPACE, " ");
  check_token(1, 12, tBAR, "|");
  check_token(1, 13, tIDENTIFIER, "n");
  check_token(1, 14, tBAR, "|");
  check_token(1, 15, tNEWLINE, "\n");
  check_token(2, 0, tSPACE, "  ");
  check_token(2, 2, tIDENTIFIER, "puts");
  check_token(2, 6, tSPACE, " ");
  check_token(2, 7, tIDENTIFIER, "n");
  check_token(2, 8, tNEWLINE, "\n");
  check_token(3, 0, tKEYWORD, "end");
  crb_free_lexer();
}

void test_lexer_array() {
  char* code = "array = [1,2,3]\n puts array[1]";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 19);
  ok(lexer->in_token == false);
  check_token(1, 0, tIDENTIFIER, "array");
  check_token(1, 5, tSPACE, " ");
  check_token(1, 6, tEQUAL, "=");
  check_token(1, 7, tSPACE, " ");
  check_token(1, 8, tLBRACKET, "[");
  check_token(1, 9, tINTEGER, "1");
  check_token(1, 10, tCOMMA, ",");
  check_token(1, 11, tINTEGER, "2");
  check_token(1, 12, tCOMMA, ",");
  check_token(1, 13, tINTEGER, "3");
  check_token(1, 14, tRBRACKET, "]");
  check_token(1, 15, tNEWLINE, "\n");
  check_token(2, 0, tSPACE, " ");
  check_token(2, 1, tIDENTIFIER, "puts");
  check_token(2, 5, tSPACE, " ");
  check_token(2, 6, tIDENTIFIER, "array");
  check_token(2, 11, tLBRACKET, "[");
  check_token(2, 12, tINTEGER, "1");
  check_token(2, 13, tRBRACKET, "]");
  crb_free_lexer();
}

void test_lexer_string_double_quote() {
  char* code = "puts \"Hello, World!\"";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 5);
  ok(lexer->in_token == false);
  check_token(1, 0, tIDENTIFIER, "puts");
  check_token(1, 4, tSPACE, " ");
  check_token(1, 5, tSTRING_BEGINING, "\"");
  check_token(1, 6, tSTRING_CONTENT, "Hello, World!");
  check_token(1, 19, tSTRING_END, "\"");
  crb_free_lexer();
}

void test_lexer_string_single_quote() {
  char* code = "puts 'Hello, World!'";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 5);
  ok(lexer->in_token == false);
  check_token(1, 0, tIDENTIFIER, "puts");
  check_token(1, 4, tSPACE, " ");
  check_token(1, 5, tSTRING_BEGINING, "'");
  check_token(1, 6, tSTRING_CONTENT, "Hello, World!");
  check_token(1, 19, tSTRING_END, "'");
  crb_free_lexer();
}

void test_lexer_colon() {
  char* code = ":hello\n::hello\n ::hello";
  crb_init_lexer(code);
  crb_lexer_lex();
  cmp_ok(lexer->num_tokens, "==", 9);
  ok(lexer->in_token == false);
  check_token(1, 0, tSYMBOL_BEGINING, ":");
  check_token(1, 1, tIDENTIFIER, "hello");
  check_token(1, 6, tNEWLINE, "\n");
  check_token(2, 0, tCOLON2, "::");
  check_token(2, 2, tIDENTIFIER, "hello");
  check_token(2, 7, tNEWLINE, "\n");
  check_token(3, 0, tSPACE, " ");
  check_token(3, 1, tCOLON3, "::");
  check_token(3, 3, tIDENTIFIER, "hello");
  crb_free_lexer();
}

void test_lexer_brace() {
  char* code = "{1}";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 3);
  ok(lexer->in_token == false);
  check_token(1, 0, tLBRACE, "{");
  check_token(1, 1, tINTEGER, "1");
  check_token(1, 2, tRBRACE, "}");
  crb_free_lexer();
}

void test_lexer_eof() {
  char* code = "1\0 2";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 1);
  ok(lexer->in_token == false);
  check_token(1, 0, tINTEGER, "1");
  crb_free_lexer();
}

void test_lexer_exclamation() {
  char* code = "!";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 1);
  check_token(1, 0, tNOT, "!");
  crb_free_lexer();

  code = "!=";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 1);
  check_token(1, 0, tNOT_EQUAL, "!=");
  crb_free_lexer();

  code = "!~";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 1);
  check_token(1, 0, tNOT_MATCH, "!~");
  crb_free_lexer();
}

void test_lexer_greater_than() {
  char* code = "1>2\n1>>2\n1>>=2\n1>=2\n";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 16);
  ok(lexer->in_token == false);
  check_token(1, 0, tINTEGER, "1");
  check_token(1, 1, tGREATER_THAN, ">");
  check_token(1, 2, tINTEGER, "2");
  check_token(1, 3, tNEWLINE, "\n");
  check_token(2, 0, tINTEGER, "1");
  check_token(2, 1, tRIGHT_SHIFT, ">>");
  check_token(2, 3, tINTEGER, "2");
  check_token(2, 4, tNEWLINE, "\n");
  check_token(3, 0, tINTEGER, "1");
  check_token(3, 1, tOP_ASSIGN, ">>=");
  check_token(3, 4, tINTEGER, "2");
  check_token(3, 5, tNEWLINE, "\n");
  check_token(4, 0, tINTEGER, "1");
  check_token(4, 1, tGREATER_THAN_OR_EQUAL, ">=");
  check_token(4, 3, tINTEGER, "2");
  check_token(4, 4, tNEWLINE, "\n");
  crb_free_lexer();
}

void test_lexer_identifiers_and_constants() {
  char* code = "a\n@a\n@@a\na?\nA";
  crb_init_lexer(code);
  crb_lexer_lex();
  ok(lexer->num_tokens == 9);
  ok(lexer->in_token == false);
  check_token(1, 0, tIDENTIFIER, "a");
  check_token(1, 1, tNEWLINE, "\n");
  check_token(2, 0, tINSTANCE_VAR, "@a");
  check_token(2, 2, tNEWLINE, "\n");
  check_token(3, 0, tCLASS_VAR, "@@a");
  check_token(3, 3, tNEWLINE, "\n");
  check_token(4, 0, tIDENTIFIER, "a?");
  check_token(4, 2, tNEWLINE, "\n");
  check_token(5, 0, tCONSTANT, "A");
  crb_free_lexer();
}

void test_lexer_greeter() {
  char* code = code = "class Greeter\n   def initialize(name = \"World\")\n     @name = name\n   end\n\n   def say_hi\n     puts \"Hi #{@name}!\"\n   end\n   def say_bye\n     puts \"Bye #{@name}, come back soon.\"\n   end\n end";
  crb_init_lexer(code);
  // For now, just test that it can parse it without running into problems
  crb_lexer_lex();
  crb_free_lexer();
}

void test_lexer_expr_begin() {
  char* code = "";
  crb_init_lexer(code);
  crb_lexer_lex();
  cmp_ok(lexer->state, "==", EXPR_BEG);
  crb_free_lexer();
}

void test_lexer_expr_mid() {
  char* code = "return";
  crb_init_lexer(code);
  crb_lexer_lex();
  cmp_ok(lexer->state, "==", EXPR_MID);
  crb_free_lexer();
}

void test_lexer_expr_class() {
  char* code = "class";
  crb_init_lexer(code);
  crb_lexer_lex();
  cmp_ok(lexer->state, "==", EXPR_CLASS);
  crb_free_lexer();
}

void test_lexer_expr_end() {
  char* code = "\"test\"";
  crb_init_lexer(code);
  crb_lexer_lex();
  cmp_ok(lexer->state, "==", EXPR_END);
  crb_free_lexer();
}

void crb_run_lexer_tests() {
  test_lexer_method_block();
  test_lexer_array();
  test_lexer_string_double_quote();
  test_lexer_string_single_quote();
  test_lexer_colon();
  test_lexer_brace();
  test_lexer_eof();
  test_lexer_exclamation();
  test_lexer_greater_than();
  test_lexer_identifiers_and_constants();
  test_lexer_greeter();
  test_lexer_expr_begin();
  test_lexer_expr_mid();
  test_lexer_expr_class();
  test_lexer_expr_end();  
}
