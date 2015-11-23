#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

Token* new_token(char* code);
void add_token(Token* token);

void crb_init_lexer() {
  lexer = (Lexer*)malloc(sizeof(Lexer));
  lexer->head = NULL;
  lexer->tail = NULL;
  lexer->in_token = false;
  lexer->num_tokens = 0;
  lexer->curr_lineno = 1;
  lexer->curr_start_pos = 0;
  lexer->curr_end_pos = 0;
}

void crb_free_lexer() {
  //TODO: free each token
  free(lexer);
}

void crb_lexer_lex(char* code) {
  int len = strlen(code);
  for (int i = 0; i < len; i++) {
    if (isdigit(code[i])) {
      if (lexer->in_token == false) {
        // start of number
        lexer->in_token = true;
        lexer->curr_type = INTEGER;
      }
      lexer->curr_end_pos++;
    } else {
      lexer->in_token = false;
      Token* token = new_token(code);
      add_token(token);
      break;
    }
  }
}

void add_token(Token* token) {
  lexer->curr_start_pos = lexer->curr_end_pos;
  if (lexer->head == NULL) {
    lexer->head = lexer->tail = token;
  } else {
    lexer->tail->next = token;
    lexer->tail = token;
  }
  lexer->num_tokens++;
}

Token* new_token(char* code) {
  Token* token = (Token*)malloc(sizeof(Token));
  token->start = lexer->curr_start_pos;
  token->lineno = lexer->curr_lineno;
  token->next = NULL;
  int token_length = lexer->curr_end_pos - lexer->curr_start_pos;
  token->value = (char *)malloc(sizeof(char) * token_length+1);
  token->value[token_length] = '\0';
  token->type = lexer->curr_type;
  strncpy(token->value, code+lexer->curr_start_pos, token_length);
  return token;
}
