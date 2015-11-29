#include <stdbool.h>
#include "lexer.h"

typedef struct parser {
  Lexer* lexer;
  bool debug;
} Parser;

// the primary lexer struct
Parser* parser;

// functions to direct the parser
void crb_init_parser();
void crb_parser_parse();
void crb_free_parser();
