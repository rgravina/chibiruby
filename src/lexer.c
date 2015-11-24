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
void consume_whitespace(char* code);
bool is_keyword(Token* token);
bool is_operator(Token* token);
bool valid_identifier_char(char curr_char);

void crb_init_lexer() {
  lexer = (Lexer*)malloc(sizeof(Lexer));
  lexer->head = NULL;
  lexer->tail = NULL;
  lexer->in_token = false;
  lexer->num_tokens = 0;
  lexer->curr_lineno = 1;
  lexer->curr_pos = 0;
  lexer->newline_last_seen_pos = 0;
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

    //
    // Handle detecting the end of token (or continuing it)
    //
    if (lexer->in_token == true) {
      // handle numbers which require some lookahead
      if (lexer->curr_type == INTEGER) {
        if (curr_char == '.') {
          char next_char = peek(code);
          if (isdigit(next_char)) {
            // it's a float, so keep going
            lexer->curr_type = FLOAT;
            lexer->curr_end_pos++;
          } else {
            // add integer, and go back before the period
            Token* token = new_token(code);
            add_token(token);
            pushback();
          }          
        } else {
          if (!isdigit(curr_char)) {
            Token* token = new_token(code);
            add_token(token);
            pushback();            
          } else {
            lexer->curr_end_pos++;            
          }
        }
      } else if (lexer->curr_type == SPACE) {
        if (isspace(curr_char)) {
          if (curr_char == '\n') {
            lexer->newline_last_seen_pos = lexer->curr_pos+1;
            lexer->curr_lineno++;
          }
          lexer->curr_end_pos++;
        } else {
          Token* token = new_token(code);
          add_token(token);
          pushback();
        }
      } else if (lexer->curr_type == IDENTIFIER) {
        if (!valid_identifier_char(curr_char)) {
          Token* token = new_token(code);
          add_token(token);
          pushback();
        } else {
          lexer->curr_end_pos++;
        }
      } else {
        // just keep going with this token then
        lexer->curr_end_pos++;
      }
    }
    //
    // Handle the start of a new token
    //
    else {
      // handle checks which can't be done in switch
      if (isdigit(curr_char)) {
        lexer->curr_type = INTEGER;
        lexer->in_token = true;
        lexer->curr_end_pos++;
      } else if (curr_char != '\n' && isspace(curr_char)) {
        lexer->curr_type = SPACE;
        lexer->in_token = true;
        lexer->curr_end_pos++;
      // TODO: handle constants (first char is A-Z)
      // handle remaining checks which can only require looking at curr_char
      } else {
        Token* token;
        switch(curr_char) {
          case '.':
            lexer->curr_type = PERIOD;
            lexer->curr_end_pos++;
            token = new_token(code);
            add_token(token);
            break;
          case '\n':
            lexer->curr_type = NEWLINE;
            lexer->curr_end_pos++;
            token = new_token(code);
            add_token(token);
            lexer->newline_last_seen_pos = lexer->curr_pos+1;
            lexer->curr_lineno++;
            break;
          case '(':
            lexer->curr_type = LPAREN;
            lexer->curr_end_pos++;
            token = new_token(code);
            add_token(token);
            break;
          case ')':
            lexer->curr_type = RPAREN;
            lexer->curr_end_pos++;
            token = new_token(code);
            add_token(token);
            break;
          case '[':
            lexer->curr_type = LBRACKET;
            lexer->curr_end_pos++;
            token = new_token(code);
            add_token(token);
            break;
          case ']':
            lexer->curr_type = RBRACKET;
            lexer->curr_end_pos++;
            token = new_token(code);
            add_token(token);
            break;
          case ',':
            lexer->curr_type = COMMA;
            lexer->curr_end_pos++;
            token = new_token(code);
            add_token(token);
            break;
          // TODO: handle other operators
          case '|':
            lexer->curr_type = OPERATOR;
            lexer->curr_end_pos++;
            token = new_token(code);
            add_token(token);
            break;
          case '=':
            lexer->curr_type = OPERATOR;
            lexer->curr_end_pos++;
            token = new_token(code);
            add_token(token);
            break;
          default:
            lexer->curr_type = IDENTIFIER;
            lexer->in_token = true;
            lexer->curr_end_pos++;
        } // switch
      } // if
    } // if
  } // for
  // add the final token if there is one
  if (lexer->in_token == true) {
    Token* token = new_token(code);
    add_token(token);
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
  token->start = (lexer->curr_start_pos - lexer->newline_last_seen_pos);
  token->lineno = lexer->curr_lineno;
  token->next = NULL;
  int token_length = lexer->curr_end_pos - lexer->curr_start_pos;
  token->value = (char *)malloc(sizeof(char) * token_length+1);
  token->value[token_length] = '\0';
  strncpy(token->value, code+lexer->curr_start_pos, token_length);
  token->type = lexer->curr_type;
  if (token->type == IDENTIFIER && is_keyword(token)) {
    token->type = KEYWORD;
  }
  return token;
}

static const char *TypeString[] = {
  "None", "Integer", "Float", "Period", "Identifier", "Space", "Keyword", "Operator", "Newline",
  "Left Paren", "Right Paren", "Left Bracket", "Right Bracket", "Comma"
};
void print_token(Token* token) {
  printf("-- token %s '%s' at (%d, %d)\n", TypeString[token->type], token->value, token->lineno, token->start);
}

char peek(char* code) {
  return code[lexer->curr_pos+1];
}

void pushback() {
  lexer->in_token = false;
  lexer->curr_pos--;
}

bool valid_identifier_char(char curr_char) {
  //FIXME: use proper Ruby leixcal rules for identifiers
  return isalpha(curr_char);
}

/*
 * FIXME: This is a tempory solution to looking up keywords and operators.
 * Should use use a hash table, maybe something generated by `gperf`.
 */
#define NUM_KEYWORDS 41
static char* KEYWORDS[NUM_KEYWORDS] = {"__ENCODING__", "__LINE__", "__FILE__",
"BEGIN", "END", "alias", "and", "begin", "break", "case", "class", "def",
"defined?", "do", "else", "elsif", "end", "ensure", "false", "for", "if", "in",
"module", "next", "nil", "not", "or", "redo", "rescue", "retry", "return",
"self", "super", "then", "true", "undef", "unless", "until", "when", "while",
"yield"};
bool is_keyword(Token* token) {
  char* value = token->value;
  for(int i = 0; i < NUM_KEYWORDS; i++) {
    if (strcmp(value, KEYWORDS[i]) == 0) {
      return true;
    }
  }
  return false;
}

#define NUM_OPERATORS 29
static char* OPERATORS[NUM_OPERATORS] = {"..", "...", "**", "**",
"<=>", "+", "-", "*", "/", "%", "<<", ">>", "<", "<=", ">", "<=", ">", ">=",
"==", "===", "!=", "!", "`", "=~", "!~", "::", "&&", "||", "&."};
bool is_operator(Token* token) {
  char* value = token->value;
  for(int i = 0; i < NUM_OPERATORS; i++) {
    if (strcmp(value, OPERATORS[i]) == 0) {
      return true;
    }
  }
  return false;
}
