void crb_init_lexer();
void crb_free_lexer();
void crb_lexer_lex(char* code);

typedef struct INTEGER {
  int lineno;
  int start;
  char* value;
} tINTEGER;

typedef struct lexer {
  tINTEGER* tokens[100];
  int tokens_length;
} Lexer;

Lexer* lexer;
