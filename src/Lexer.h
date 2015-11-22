void crb_init_lexer();
void crb_free_lexer();
void crb_lexer_lex(char* code);

typedef struct integer {
  char* value;
} Integer;

typedef struct token {
  Integer* token;
  int lineno;
  int start;
  struct token* next;
} Token;

typedef struct lexer {
  Token* token;
  int tokens_length;
} Lexer;

Lexer* lexer;
