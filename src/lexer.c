#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

Token* new_token();
void add_token(Token* token);
char peek();
void pushback();
void print_token(Token* token);
bool is_keyword(Token* token);
bool is_operator(Token* token);
bool valid_identifier_char();
void process_inside_token();
void start_next_token();
void add_token_here(Type type);
void start_new_token(Type type);

void crb_init_lexer(char* code) {
  lexer = (Lexer*)malloc(sizeof(Lexer));
  lexer->print_tokens = false;
  lexer->head = NULL;
  lexer->tail = NULL;
  lexer->in_token = false;
  lexer->code = code;
  lexer->num_tokens = 0;
  lexer->curr_lineno = 1;
  lexer->curr_pos = 0;
  lexer->newline_last_seen_pos = 0;
  lexer->curr_start_pos = 0;
  lexer->curr_end_pos = 0;
}

static Token* curr_token;
Token* crb_next_token() {
  if (curr_token == NULL) {
    curr_token = lexer->head;
  } else {
    curr_token = curr_token->next;
  }
  return curr_token;
}

void crb_free_lexer() {
  Token* token = lexer->head;
  while (token != NULL) {
    Token* temp = token;
    token = token->next;
    free(temp);
  }
  free(lexer);
  curr_token = NULL;
}

void crb_lexer_lex() {
  int len = strlen(lexer->code);
  for (lexer->curr_pos = 0; lexer->curr_pos < len; lexer->curr_pos++) {
    lexer->curr_char = lexer->code[lexer->curr_pos];
    if (lexer->in_token == true) {
      process_inside_token();
    } else {
      start_next_token();
    }
  }
  // add the final token if there is one
  if (lexer->in_token == true) {
    add_token(new_token());
  }
}

void process_inside_token() {
  // handle numbers which require some lookahead
  switch(lexer->curr_type) {
    case INTEGER:
      if (lexer->curr_char == '.') {
        char next_char = peek();
        if (isdigit(next_char)) {
          // it's a float, so keep going
          lexer->curr_type = FLOAT;
          lexer->curr_end_pos++;
        } else {
          // add integer, and go back before the period
          Token* token = new_token();
          add_token(token);
          pushback();
        }
      } else {
        if (!isdigit(lexer->curr_char)) {
          Token* token = new_token();
          add_token(token);
          pushback();
        } else {
          lexer->curr_end_pos++;
        }
      }
      break;
    case SPACE:
      if (isspace(lexer->curr_char)) {
        if (lexer->curr_char == '\n') {
          lexer->newline_last_seen_pos = lexer->curr_pos+1;
          lexer->curr_lineno++;
        }
        lexer->curr_end_pos++;
      } else {
        Token* token = new_token();
        add_token(token);
        pushback();
      }
      break;
    case IDENTIFIER:
      if (!valid_identifier_char(lexer->curr_char)) {
        Token* token = new_token();
        add_token(token);
        pushback();
      } else {
        lexer->curr_end_pos++;
      }
      break;
    case STRING_CONTENT:
      if ((lexer->curr_char == '\"' && strcmp(lexer->tail->value, "\"") == 0) ||
          (lexer->curr_char == '\'' && strcmp(lexer->tail->value, "\'") == 0)) {
        Token* token = new_token();
        add_token(token);
        add_token_here(STRING_END);
      } else {
        lexer->curr_end_pos++;
      }
      break;
    default:
      // just keep going with this token then
      lexer->curr_end_pos++;
  } // switch
}

void add_token_here(Type type) {
  lexer->curr_type = type;
  lexer->curr_end_pos++;
  add_token(new_token(lexer->code));
}

void start_new_token(Type type) {
  lexer->curr_type = type;
  lexer->in_token = true;
}

void start_next_token() {
  /*
   * token :: keyword | identifier | punctuator | operator | literal
   */
  // handle checks which can't be done in switch
  if (isdigit(lexer->curr_char)) {
    start_new_token(INTEGER);
    lexer->curr_end_pos++;
  } else if (lexer->curr_char != '\n' && isspace(lexer->curr_char)) {
    start_new_token(SPACE);
    lexer->curr_end_pos++;
  // TODO: handle constants (first char is A-Z)
  // handle remaining checks which can only require looking at lexer->curr_char
  } else {
    switch(lexer->curr_char) {
      case '.':
        add_token_here(PERIOD);
        break;
      case '\n':
        add_token_here(NEWLINE);
        lexer->newline_last_seen_pos = lexer->curr_pos+1;
        lexer->curr_lineno++;
        break;
      case '(':
        add_token_here(LPAREN);
        break;
      case ')':
        add_token_here(RPAREN);
        break;
      case '[':
        add_token_here(LBRACKET);
        break;
      case ']':
        add_token_here(RBRACKET);
        break;
      case ',':
        add_token_here(COMMA);
        break;
      case '\'':
      case '\"':
        add_token_here(STRING_BEGINING);
        start_new_token(STRING_CONTENT);
        break;
      // TODO: handle other operators
      case '|':
        add_token_here(OPERATOR);
        break;
      case '=':
        add_token_here(OPERATOR);
        break;
      default:
        start_new_token(IDENTIFIER);
        lexer->curr_end_pos++;
    } // switch
  } // if  
}

void add_token(Token* token) {
  if (lexer->print_tokens) {
    print_token(token);
  }
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

Token* new_token() {
  Token* token = (Token*)malloc(sizeof(Token));
  token->start = (lexer->curr_start_pos - lexer->newline_last_seen_pos);
  token->lineno = lexer->curr_lineno;
  token->next = NULL;
  int token_length = lexer->curr_end_pos - lexer->curr_start_pos;
  token->value = (char *)malloc(sizeof(char) * token_length+1);
  token->value[token_length] = 0;
  strncpy(token->value, lexer->code+lexer->curr_start_pos, token_length);
  token->type = lexer->curr_type;
  if (token->type == IDENTIFIER && is_keyword(token)) {
    token->type = KEYWORD;
  }
  return token;
}

static const char *TypeString[] = {
  "None", "Integer", "Float", "Period", "Identifier", "Space", "Keyword", "Operator", "Newline",
  "Left Paren", "Right Paren", "Left Bracket", "Right Bracket", "Comma", "String Start", "String Content",
  "String End"
};
void print_token(Token* token) {
  printf("-- token %s '%s' at (%d, %d)\n", TypeString[token->type], token->value, token->lineno, token->start);
}

char peek() {
  return lexer->code[lexer->curr_pos+1];
}

void pushback() {
  lexer->in_token = false;
  lexer->curr_pos--;
}

bool valid_identifier_char() {
  //FIXME: use proper Ruby leixcal rules for identifiers
  return isalpha(lexer->curr_char);
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
