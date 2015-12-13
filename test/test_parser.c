#include <stdio.h>
#include <string.h>
#include "tap.h"
#include "parser.h"
#include "node.h"

void test_parser() {
  char* code = "10.times do |n|\n  puts n\nend";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_simple_expressions() {
  char* code = "1\n!2\n3+1\n+4\n5-6\n~7\n8..9\n0...1\n2*3\n4/5\n2+3-5*2\n-1\n1%2\n3**4\n5|6\n7^8\n1&2\n1&&2";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_more_expressions() {
  char* code = "1&&2||3\n1..2+3\n1||2&&3<4<=5--6\n1>2&&2>=3>>4\n5==6\n7===8\n9=~0";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_lhs() {
  // add some expressions in here to make sure the parser resets after lhs OK
  char* code = "a=2\n1&&2@a=2\n1==2\n@@a=1";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_lhs_extras() {
  // seems this is syntactially valid but a semantic error
  char* code = "a[]=1\na.b=2";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_primary_varname() {
  char* code = "a=b";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_primary_paren_extras() {
  char* code = "(a+b)+(a+b)\n::a";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_primary_class() {
  char* code = "class Dog < Animal\nend";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_primary_begin_end() {
  char* code = "begin a=1+2 end";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_stmt_trailing_condition() {
  char* code = "a=1+2 if a==1\na=1+2 unless a==1\na=1+2 while a==1\na=1+2 until a==1";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_stmt_trailing_rescue() {
  char* code = "a=1+2 rescue a=1";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_expr_call() {
  char* code = "puts 1";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
}

void test_parser_simple_node() {
  char* code = "1\n:sym";
  crb_init_parser(code);
  crb_parser_parse();
  crb_free_parser();
  crb_node_print_tree();
}

void test_parser_string_node() {
  char* code = "\"a\"";
  crb_init_parser(code);
  lexer->debug = true;
  parser->debug = true;
  crb_parser_parse();
  crb_free_parser();
  crb_node_print_tree();
}

void crb_run_parser_tests() {
  test_parser_simple_expressions();
  test_parser_more_expressions();
  test_parser_lhs();
  test_parser_lhs_extras();
  test_parser_primary_varname();
  test_parser_primary_paren_extras();
  test_parser_primary_class();
  test_parser_primary_begin_end();
  test_parser_stmt_trailing_condition();
  test_parser_stmt_trailing_rescue();
  test_parser_expr_call();
  test_parser_simple_node();
  test_parser_string_node();
}
