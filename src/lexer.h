#include <stdbool.h>

typedef enum type {NONE, INTEGER, FLOAT, PERIOD, IDENTIFIER, SPACE, KEYWORD, NEWLINE, LPAREN, RPAREN, LBRACKET, RBRACKET, COMMA, STRING_BEGINING, STRING_CONTENT, STRING_END,
LBRACE, RBRACE, SYMBOL_BEGINING, COLON2, BAR, NOT, EQUAL, NOT_EQUAL, NOT_MATCH, RIGHT_SHIFT, OP_ASSIGN,
GREATER_THAN, GREATER_THAN_OR_EQUAL, COLON3, INSTANCE_VAR, CLASS_VAR} Type;

typedef enum state {EXPR_BEG, EXPR_FNAME, EXPR_DOT, EXPR_CLASS} State;

typedef struct token {
  char* value;
  Type type;
  unsigned long lineno;
  unsigned long start;
  struct token* next;
} Token;

typedef struct lexer {
  Token* head;
  Token* tail;
  Type curr_type;
  State state;
  char* code;
  char curr_char;
  bool print_tokens;
  bool in_token;
  unsigned long curr_pos;
  unsigned long num_tokens;
  unsigned long curr_lineno;
  unsigned long newline_last_seen_pos;
  unsigned long curr_start_pos;
  unsigned long curr_end_pos;
} Lexer;

// the primary lexer struct
Lexer* lexer;

// functions to direct the lexer
void crb_init_lexer(char* code);
void crb_free_lexer();
void crb_lexer_lex();
Token* crb_next_token();
