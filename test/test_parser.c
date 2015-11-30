#include <stdio.h>
#include <string.h>
#include "tap.h"
#include "parser.h"

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
  char* code = "1&&2||3\n1..2+3\n1||2&&3<4<=5--6";
  crb_init_parser(code);
  parser->debug=true;
  crb_parser_parse();
  crb_free_parser();
}

void crb_run_parser_tests() {
  test_parser_simple_expressions();
  test_parser_more_expressions();
}
