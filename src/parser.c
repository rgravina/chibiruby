#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

void crb_init_parser(char* code) {
  crb_init_lexer(code);
  parser = (Parser*)malloc(sizeof(Parser));
  parser->lexer = lexer;
}

void crb_parser_parse() {
  crb_lexer_lex();
  Token* token = crb_next_token();
  while (token != NULL) {
    token = crb_next_token();
  }
}

void crb_free_parser() {
  crb_free_lexer();
  free(parser);
}

