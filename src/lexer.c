#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

Token* new_token(char* code);
void add_token(Token* token);
char peek(char* code);
void pushback();
void print_token(Token* token);

void crb_init_lexer() {
  lexer = (Lexer*)malloc(sizeof(Lexer));
  lexer->head = NULL;
  lexer->tail = NULL;
  lexer->in_token = false;
  lexer->num_tokens = 0;
  lexer->curr_lineno = 1;
  lexer->curr_pos = 0;
  lexer->curr_start_pos = 0;
  lexer->curr_end_pos = 0;
}

void crb_free_lexer() {
  Token* token = lexer->head;
  while (token != NULL) {
    Token* temp = token;
    token = token->next;
    free(temp);
  }
  free(lexer);
}

void crb_lexer_lex(char* code) {
  int len = strlen(code);
  for (lexer->curr_pos = 0; lexer->curr_pos < len; lexer->curr_pos++) {
    char curr_char = code[lexer->curr_pos];

    if (isdigit(curr_char)) {
      if (lexer->in_token == false) {
        // start of number
        lexer->in_token = true;
        lexer->curr_type = INTEGER;
      }
      lexer->curr_end_pos++;
    } else if (curr_char == '.') {
      char next_char = peek(code);
      if (lexer->curr_type == INTEGER) {
        // decimal point
        if (isdigit(next_char)) {
          lexer->curr_type = FLOAT;
          lexer->curr_end_pos++;
        } else {
          Token* token = new_token(code);
          add_token(token);
          pushback();
        }
      } else {
        lexer->curr_type = PERIOD;
        lexer->curr_end_pos++;
        Token* token = new_token(code);
        add_token(token);
        break;
      }
    } else {
      Token* token = new_token(code);
      add_token(token);
    }
  }
}

void add_token(Token* token) {
  //print_token(token);
  lexer->in_token = false;
  lexer->curr_type = NONE;
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

static const char *TypeString[] = {
  "None", "Integer", "Float", "Period",
};
void print_token(Token* token) {
  printf("***** token (%s) ****\n", TypeString[token->type]);
  printf("Start Pos: (%d)\n", token->start);
}

char peek(char* code) {
  return code[lexer->curr_pos+1];
}

void pushback() {
  lexer->in_token = false;
  lexer->curr_pos--;
}
