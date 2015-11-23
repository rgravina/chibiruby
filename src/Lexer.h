void crb_init_lexer();
void crb_free_lexer();
void crb_lexer_lex(char* code);

typedef struct token {
  char* value;
  int lineno;
  int start;
  struct token* next;
} Token;

typedef struct lexer {
  Token* head;
  Token* tail;
  int num_tokens;
  int curr_lineno;
  int curr_start_pos;
  int curr_end_pos;
} Lexer;

Lexer* lexer;
