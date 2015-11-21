char* crb_lexer_lex(char* code);

// an integer
typedef struct INTEGER {
  int lineno;
  int start;
  char* value;
} tINTEGER;

tINTEGER* tokens[100];
