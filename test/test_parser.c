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

void test_parser_literal() {
  // tests statements with newlines
  char* code = "1\n2\n3\n4";
  crb_init_parser(code);
  parser->debug=true;
  crb_parser_parse();
  crb_free_parser();
}

void crb_run_parser_tests() {
  test_parser_literal();
}
