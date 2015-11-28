#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

void parse_program();
void parse_top_compstmt();
void parse_top_stmts();
void parse_top_stmt();
void parse_opt_terms();
void parse_terms();
void parse_none();
void parse_error();

void crb_init_parser(char* code) {
  crb_init_lexer(code);
  parser = (Parser*)malloc(sizeof(Parser));
  parser->lexer = lexer;
}

/*
 * Parser initialise/free function
 */
void crb_parser_parse() {
  crb_lexer_lex();
  parse_program();
}

void crb_free_parser() {
  crb_free_lexer();
  free(parser);
}

/*
 * Grammar parsing functions
 */
void parse_program() {
  // program : top_compstmt
  parse_top_compstmt();
}

void parse_top_compstmt() {
  // top_compstmt : top_stmts opt_terms

  /*
   * A compound-statement is evaluated as follows:
   *
   * If the statement-list of the compound-statement is omitted, the value of
   * the compound-statement is nil.
   *
   * if the statement-list exists, the value of the compound-statement is the
   * value of the last statement of the statement-list.
   *
   */
  parse_top_stmts();
  parse_opt_terms();
}

void parse_top_stmts() {
  /*
  top_stmts : none
  		| top_stmt
  		| top_stmts terms top_stmt
  		| error top_stmt
  */
  // parse_none();
  // parse_top_stmt();
  // parse_top_stmts();
  // parse_terms();
  // parse_top_stmt();
  // parse_error();
  // parse_top_stmt();
}

void parse_top_stmt() {
}

void parse_opt_terms() {
}

void parse_terms() {
}

void parse_none() {
}

void parse_error() {
}
