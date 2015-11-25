#include <stdbool.h>

void crb_init_lexer(char* code);
void crb_free_lexer();
void crb_lexer_lex();

typedef enum type {NONE, INTEGER, FLOAT, PERIOD, IDENTIFIER, SPACE, KEYWORD, NEWLINE, LPAREN, RPAREN, LBRACKET, RBRACKET, COMMA, STRING_BEGINING, STRING_CONTENT, STRING_END,
LBRACE, RBRACE, SYMBOL_BEGINING, COLON2, BAR, NOT, EQUAL, NOT_EQUAL, NOT_MATCH, RIGHT_SHIFT, OP_ASSIGN,
GREATER_THAN, GREATER_THAN_OR_EQUAL} Type;

typedef struct token {
  char* value;
  Type type;
  int lineno;
  int start;
  struct token* next;
} Token;

Token* crb_next_token();

typedef struct lexer {
  Token* head;
  Token* tail;
  int print_tokens;
  int num_tokens;
  bool in_token;
  Type curr_type;
  char* code;
  char curr_char;
  int curr_lineno;
  int curr_pos;
  int newline_last_seen_pos;
  int curr_start_pos;
  int curr_end_pos;
} Lexer;

Lexer* lexer;
