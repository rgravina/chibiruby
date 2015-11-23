#include <stdbool.h>

void crb_init_lexer();
void crb_free_lexer();
void crb_lexer_lex(char* code);

typedef enum type {NONE, INTEGER, FLOAT, PERIOD, IDENTIFIER, SPACE, KEYWORD} Type;

typedef struct token {
  char* value;
  Type type;
  int lineno;
  int start;
  struct token* next;
} Token;

typedef struct lexer {
  Token* head;
  Token* tail;
  int num_tokens;
  bool in_token;
  Type curr_type;
  int curr_lineno;
  int curr_pos;
  int curr_start_pos;
  int curr_end_pos;
} Lexer;

Lexer* lexer;
