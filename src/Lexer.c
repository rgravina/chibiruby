#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

// token array
extern tINTEGER* tokens[100];
int tokens_length = 0;

void crb_lexer_lex(char* code) {
  int len = strlen(code);
  int lineno = 1;
  int token_start_pos = 0;
  int token_end_pos = 0;
  for (int i = 0; i < len; i++) {
    if (isdigit(code[i])) {
      token_end_pos++;
    } else {
      tINTEGER* token = (tINTEGER*)malloc(sizeof(tINTEGER));
      int token_length = token_end_pos-token_start_pos;
      token->lineno = lineno;
      token->start = token_start_pos;
      token->value = (char *)malloc(sizeof(char) * token_length+1);
      token->value[token_length] = '\0';
      strncpy(token->value, code+token_start_pos, token_length);
      token_start_pos = token_end_pos;
      tokens[tokens_length++] = token;
      break;
    }
  }
}
