#include <stdbool.h>

typedef enum tokenType {tNONE, tINTEGER, tFLOAT, tPERIOD, tIDENTIFIER, tSPACE,
  tKEYWORD, tNEWLINE, tLPAREN, tRPAREN, tLBRACKET, tRBRACKET, tCOMMA,
  tSTRING_BEGINING, tSTRING_CONTENT, tSTRING_END, tLBRACE, tRBRACE,
  tSYMBOL_BEGINING, tCOLON2, tBAR, tNOT, tEQUAL, tNOT_EQUAL, tNOT_MATCH,
  tRIGHT_SHIFT, tOP_ASSIGN, tGREATER_THAN, tGREATER_THAN_OR_EQUAL, tCOLON3,
  tINSTANCE_VAR, tCLASS_VAR, tIGNORED_NEWLINE, tCONSTANT, tSEMICOLON,
  tPLUS, tUPLUS, tMINUS, tUMINUS, tLAMBDA, tTILDE, tDOT2, tDOT3, tMULTIPLY,
  tDIVIDE, tPOW, tREGEXP_BEG, tPERCENT, tCARET, tAMPER, tAND_OP, tAND_DOT} TokenType;

typedef enum state {
  EXPR_BEG,
  EXPR_MID,
  EXPR_ARG,
  // EXPR_CMDARG,
  // EXPR_ENDARG,
  EXPR_END,
  EXPR_FNAME,
  EXPR_DOT,
  EXPR_CLASS
} State;

typedef struct token {
  char* value;
  TokenType type;
  unsigned long lineno;
  unsigned long start;
  struct token* next;
  struct token* previous;
} Token;

typedef struct lexer {
  Token* head;
  Token* tail;
  TokenType curr_type;
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
Token* crb_previous_token();
Token* crb_curr_token();
