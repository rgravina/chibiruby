#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

void crb_init_lexer() {
  lexer = (Lexer*)malloc(sizeof(Lexer));
  lexer->tokens_length = 0;
}

void crb_free_lexer() {
  free(lexer);
}

void crb_lexer_lex(char* code) {
  int len = strlen(code);
  int lineno = 1;
  int token_start_pos = 0;
  int token_end_pos = 0;
  for (int i = 0; i < len; i++) {
    if (isdigit(code[i])) {
      token_end_pos++;
    } else {
      Token* token = (Token*)malloc(sizeof(Token));
      Integer* integer = (Integer*)malloc(sizeof(Integer));
      token->token = integer;
      int token_length = token_end_pos-token_start_pos;
      token->lineno = lineno;
      token->start = token_start_pos;
      token->token->value = (char *)malloc(sizeof(char) * token_length+1);
      token->next = NULL;
      token->token->value[token_length] = '\0';
      strncpy(token->token->value, code+token_start_pos, token_length);
      token_start_pos = token_end_pos;
      lexer->tokens[lexer->tokens_length] = token;
      lexer->tokens_length++;
      break;
    }
  }
}
