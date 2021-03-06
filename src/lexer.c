#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

// lexer movement/inspection
bool should_abort_now();
char peek();
void pushback();
void advance_token_and_lexer();
void advance_token();
// token inspection
static void print_token(Token* token);
bool is_keyword(Token* token);
bool is_after_operator();
bool is_expression_beginning();
bool valid_identifier_char();
// token creation and processing
Token* new_token();
void add_token();
void add_token_here(TokenType type);
void process_short_token();
void process_long_token();
void start_long_token(TokenType type);

static bool started_walking;
void crb_init_lexer(char* code) {
  lexer = (Lexer*)malloc(sizeof(Lexer));
  lexer->debug = false;
  lexer->head = NULL;
  lexer->tail = NULL;
  lexer->state = EXPR_BEG;
  lexer->in_token = false;
  lexer->code = code;
  lexer->num_tokens = 0;
  lexer->curr_lineno = 1;
  lexer->curr_pos = 0;
  lexer->newline_last_seen_pos = 0;
  lexer->curr_start_pos = 0;
  lexer->curr_end_pos = 0;
  started_walking = false;
}

// used for iterating through the tokens from outside
static Token* curr_token;
Token* crb_next_token() {
  if (curr_token == NULL && !started_walking) {
    curr_token = lexer->head;
    started_walking = true;
  } else {
    curr_token = curr_token->next;
  }
  return curr_token;
}

Token* crb_previous_token() {
  if (curr_token == NULL) {
    return NULL;
  } else {
    curr_token = curr_token->previous;
  }
  return curr_token;
}

Token* crb_curr_token() {
  return curr_token;
}

void crb_set_token(Token* token) {
  curr_token = token;
}


void crb_free_lexer() {
  Token* token = lexer->head;
  while (token != NULL) {
    Token* temp = token;
    token = token->next;
    free(temp->value);
    free(temp);
  }
  free(lexer);
  curr_token = NULL;
}

void crb_lexer_lex() {
  while (lexer->curr_pos < strlen(lexer->code)) {
    lexer->curr_char = lexer->code[lexer->curr_pos];
    if (should_abort_now()) {
      break;
    }
    if (lexer->in_token != true) {
      process_short_token();
    } else {
      process_long_token();
    }
    lexer->curr_pos++;
  }
  // add the final token if there is one
  if (lexer->in_token == true) {
    add_token();
  }
}

bool should_abort_now() {
  switch(lexer->curr_char) {
    case '\0':    // NULL
    case '\004':  // ^D
    case '\032':  // ^Z
    case -1:      // end of script
      return true;
    default:
      return false;
  }
}

void process_long_token() {
  // handle numbers which require some lookahead
  switch(lexer->curr_type) {
    case tINTEGER:
      if (lexer->curr_char == '.') {
        char next_char = peek();
        if (isdigit(next_char)) {
          // it's a float, so keep going
          lexer->curr_type = tFLOAT;
          advance_token();
        } else {
          // add integer, and go back before the period
          add_token();
          pushback();
        }
      } else {
        if (!isdigit(lexer->curr_char)) {
          lexer->state = EXPR_END;
          add_token();
          pushback();
        } else {
          advance_token();
        }
      }
      break;
    case tSPACE:
      if (isspace(lexer->curr_char)) {
        if (lexer->curr_char == '\n') {
          lexer->newline_last_seen_pos = lexer->curr_pos+1;
          lexer->curr_lineno++;
        }
        advance_token();
      } else {
        add_token();
        pushback();
      }
      break;
    case tIDENTIFIER:
    case tCONSTANT:
    case tINSTANCE_VAR:
    case tCLASS_VAR:
    case tFID:
      if (!valid_identifier_char()) {
        lexer->state = EXPR_END;
        add_token();
        pushback();
      } else {
        if (lexer->curr_char == '!' || lexer->curr_char == '?') {
          lexer->curr_type = tFID;
        }
        advance_token();
      }
      break;
    case tSTRING_CONTENT:
      if ((lexer->curr_char == '\"' && strcmp(lexer->tail->value, "\"") == 0) ||
          (lexer->curr_char == '\'' && strcmp(lexer->tail->value, "\'") == 0)) {
        add_token();
        lexer->state = EXPR_END;
        add_token_here(tSTRING_END);
      } else {
        advance_token();
      }
      break;
    default:
      // just keep going with this token then
      advance_token();
  } // switch
}

void add_token_here(TokenType type) {
  lexer->curr_type = type;
  lexer->curr_end_pos++;
  add_token();
}

void start_long_token(TokenType type) {
  lexer->curr_type = type;
  lexer->in_token = true;
}

void process_short_token() {
  char next_char;
  switch(lexer->curr_char) {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      start_long_token(tINTEGER);
      advance_token();
      break;
    case ' ': case '\t': case '\f': case '\r': case '\13':
      start_long_token(tSPACE);
      advance_token();
      break;
    case '\n':
      switch (lexer->state) {
        case EXPR_BEG:
        case EXPR_FNAME:
        case EXPR_DOT:
        case EXPR_CLASS:
        case EXPR_ARG:
          add_token_here(tIGNORED_NEWLINE);
          break;
        default:
          lexer->state = EXPR_BEG;
          add_token_here(tNEWLINE);
          break;
      }
      lexer->newline_last_seen_pos = lexer->curr_pos+1;
      lexer->curr_lineno++;
      break;
    case '#':
      //TODO: comments
      break;
    case '+':
      next_char = peek();
      if (is_after_operator()) {
        lexer->state = EXPR_ARG;
        if (next_char == '@') {
          advance_token_and_lexer();
          add_token_here(tUPLUS);
        } else {
          add_token_here(tPLUS);
        }
      } else if (next_char == '=') {
        lexer->state = EXPR_BEG;
        advance_token_and_lexer();
        add_token_here(tOP_ASSIGN);
      } else if (is_expression_beginning()) {
        lexer->state = EXPR_BEG;
        add_token_here(tUPLUS);
      } else {
        lexer->state = EXPR_BEG;
        add_token_here(tPLUS);
      }
      break;
    case '-':
      next_char = peek();
      if (is_after_operator()) {
        lexer->state = EXPR_ARG;
        if (next_char == '@') {
          advance_token_and_lexer();
          add_token_here(tUMINUS);
        } else {
          add_token_here(tMINUS);
        }
      } else if (next_char == '=') {
        lexer->state = EXPR_ARG;
        advance_token_and_lexer();
        add_token_here(tOP_ASSIGN);
      } else if (next_char == '>') {
        // Ruby's parse.y uses EXPR_ENDFN here
        lexer->state = EXPR_END;
        advance_token_and_lexer();
        add_token_here(tLAMBDA);
      } else if (is_expression_beginning()) {
        lexer->state = EXPR_BEG;
        add_token_here(tUMINUS);
      } else {
        lexer->state = EXPR_BEG;
        add_token_here(tMINUS);
      }
      break;
    case '*':
      next_char = peek();
      if (next_char == '*') {
        advance_token_and_lexer();
        next_char = peek();
        if (next_char == '=') {
          advance_token_and_lexer();
          lexer->state = EXPR_BEG;
          add_token_here(tOP_ASSIGN);
        } else {
          add_token_here(tPOW);
        }
      } else {
        add_token_here(tMULTIPLY);
      }
      break;
    case '/':
      if (is_expression_beginning()) {
        // this is the start of a regex, but not supported yet
      }
      next_char = peek();
      if (next_char == '*') {
        advance_token_and_lexer();
        lexer->state = EXPR_BEG;
        add_token_here(tOP_ASSIGN);
      } else {
        lexer->state = is_after_operator() ? EXPR_ARG : EXPR_BEG;
        add_token_here(tDIVIDE);
      }
      break;
    case '~':
      next_char = peek();
      if (is_after_operator()) {
        if (next_char == '@') {
          pushback();
        }
        lexer->state = EXPR_ARG;
      } else {
        lexer->state = EXPR_BEG;
      }
      add_token_here(tTILDE);
      break;
    case ';':
      add_token_here(tSEMICOLON);
      break;
    case '%':
      add_token_here(tPERCENT);
      break;
    case '.':
      lexer->state = EXPR_BEG;
      next_char = peek();
      if (next_char == '.') {
        advance_token_and_lexer();
        next_char = peek();
        if (next_char == '.') {
          advance_token_and_lexer();
          add_token_here(tDOT3);
        } else {
          add_token_here(tDOT2);
        }
      } else {
        lexer->state = EXPR_DOT;
        add_token_here(tDOT);
      }
      break;
    case '(':
      lexer->state = EXPR_BEG;
      add_token_here(tLPAREN);
      break;
    case ')':
      add_token_here(tRPAREN);
      break;
    case '[':
      lexer->state = EXPR_BEG;
      add_token_here(tLBRACKET);
      break;
    case ']':
      add_token_here(tRBRACKET);
      break;
    case '{':
      lexer->state = EXPR_BEG;
      add_token_here(tLBRACE);
      break;
    case '}':
      add_token_here(tRBRACE);
      break;
    case ':':
      lexer->state = EXPR_BEG;
      next_char = peek();
      if (next_char == ':') {
        if (lexer->tail->type == tSPACE) {
          advance_token_and_lexer();
          // e.g. ::Const
          add_token_here(tCOLON3);
        } else {
          advance_token_and_lexer();
          // e.g. Net::SMTP
          add_token_here(tCOLON2);
        }
      } else {
        add_token_here(tSYMBOL_BEGINING);
      }
      break;
    case ',':
      lexer->state = EXPR_BEG;
      add_token_here(tCOMMA);
      break;
    case '\'':
    case '\"':
      add_token_here(tSTRING_BEGINING);
      start_long_token(tSTRING_CONTENT);
      break;
    case '|':
      next_char = peek();
      if (next_char == '|') {
        lexer->state = EXPR_BEG;
        advance_token_and_lexer();
        if (next_char == '=') {
          advance_token_and_lexer();
          lexer->state = EXPR_BEG;
          add_token_here(tOP_ASSIGN);
        } else {
          add_token_here(tOROP);
        }
      } else if (next_char == '=') {
        advance_token_and_lexer();
        lexer->state = EXPR_BEG;
        add_token_here(tOP_ASSIGN);
      }
      else {
        add_token_here(tBAR);
      }
      break;
    case '=':
      // TODO: handle tEMBDOC
      lexer->state = is_after_operator() ? EXPR_ARG : EXPR_BEG;
      next_char = peek();
      if (next_char == '=') {
        advance_token_and_lexer();
        next_char = peek();
        if (next_char == '=') {
          advance_token_and_lexer();
          add_token_here(tEQQ);
        } else {
          add_token_here(tEQ);
        }
      } else if (next_char == '~') {
        advance_token_and_lexer();
        add_token_here(tMATCH);
      } else if (next_char == '>') {
        advance_token_and_lexer();
        add_token_here(tASSOC);
      } else {
        add_token_here(tASSIGN);
      }
      break;
    case '!':
      next_char = peek();
      if (next_char == '=') {
        advance_token_and_lexer();
        add_token_here(tNOT_EQUAL);
      } else if (next_char == '~') {
        advance_token_and_lexer();
        add_token_here(tNOT_MATCH);
      } else {
        add_token_here(tNOT);
      }
      break;
    case '^':
      next_char = peek();
      if (next_char == '=') {
        advance_token_and_lexer();
        lexer->state = EXPR_BEG;
        add_token_here(tOP_ASSIGN);
      } else {
        lexer->state = is_after_operator() ? EXPR_ARG : EXPR_BEG;
        add_token_here(tCARET);
      }
      break;
    case '&':
      next_char = peek();
      if (next_char == '&') {
        advance_token_and_lexer();
        lexer->state = EXPR_BEG;
        if (next_char == '=') {
          add_token_here(tOP_ASSIGN);
        } else {
          add_token_here(tAND_OP);
        }
      } else if (next_char == '=') {
        advance_token_and_lexer();
        add_token_here(tOP_ASSIGN);
      } else if (next_char == '.') {
        lexer->state = EXPR_DOT;
        add_token_here(tAND_DOT);
      } else if (is_expression_beginning()) {
        add_token_here(tAMPER);
      } else {
        //FIXME: what is the difference between tAMPER and '&'?
        add_token_here(tAMPER);
      }
      break;
    case '>':
      next_char = peek();
      if (next_char == '=') {
        advance_token_and_lexer();
        add_token_here(tGEQ);
      } else if (next_char == '>') {
        advance_token_and_lexer();
        next_char = peek();
        if (next_char == '=') {
          advance_token_and_lexer();
          add_token_here(tOP_ASSIGN);
        } else {
          add_token_here(tRSHIFT);
        }
      } else {
        add_token_here(tGT);
      }
      break;
    case '<':
      next_char = peek();
      // TODO: checking for heredoc start happens here
      if (is_after_operator()) {
        lexer->state = EXPR_ARG;
      } else {
        if (lexer->state == EXPR_CLASS) {
          // not sure if I need to set this yet
          // command_start = true
        }
        lexer->state = EXPR_BEG;
      }
      if (next_char == '=') {
        advance_token_and_lexer();
        next_char = peek();
        if (next_char == '>') {
          advance_token_and_lexer();
          add_token_here(tCMP);
        } else {
          add_token_here(tLEQ);
        }
      } else if (next_char == '<') {
        advance_token_and_lexer();
        next_char = peek();
        if (next_char == '=') {
          advance_token_and_lexer();
          add_token_here(tOP_ASSIGN);
        } else {
          add_token_here(tLSHIFT);
        }
      } else {
        add_token_here(tLT);
      }
      break;
    default:
      if (lexer->curr_char == '@') {
        next_char = peek();
        if (next_char == '@') {
          advance_token_and_lexer();
          start_long_token(tCLASS_VAR);
        } else {
          advance_token_and_lexer();
          start_long_token(tINSTANCE_VAR);
        }
      } else {
        if (lexer->curr_char >= 'A' && lexer->curr_char <= 'Z') {
          start_long_token(tCONSTANT);
        } else {
          start_long_token(tIDENTIFIER);
        }
      }
      advance_token();
  } // switch
}

void advance_token_and_lexer() {
  lexer->curr_end_pos++;
  lexer->curr_pos++;
}

void advance_token() {
  lexer->curr_end_pos++;
}

void add_token() {
  Token* token = new_token();
  if (lexer->debug) {
    print_token(token);
  }
  lexer->in_token = false;
  lexer->curr_type = tNONE;
  lexer->curr_start_pos = lexer->curr_end_pos;
  if (lexer->head == NULL) {
    lexer->head = lexer->tail = token;
  } else {
    lexer->tail->next = token;
    token->previous = lexer->tail->next;
    lexer->tail = token;
  }
  lexer->num_tokens++;
}

Token* new_token() {
  Token* token = (Token*)malloc(sizeof(Token));
  token->start = (lexer->curr_start_pos - lexer->newline_last_seen_pos);
  token->lineno = lexer->curr_lineno;
  token->next = NULL;
  token->previous = NULL;
  int token_length = lexer->curr_end_pos - lexer->curr_start_pos;
  token->value = (char *)malloc(sizeof(char) * token_length+1);
  token->value[token_length] = 0;
  strncpy(token->value, lexer->code+lexer->curr_start_pos, token_length);
  token->type = lexer->curr_type;
  if (token->type == tIDENTIFIER && is_keyword(token)) {
    token->type = tKEYWORD;
    if (strcmp("return", token->value) == 0 ||
        strcmp("break", token->value) == 0 ||
        strcmp("next", token->value) == 0 ||
        strcmp("rescue", token->value) == 0) {
      lexer->state = EXPR_MID;
    } else if (strcmp("class", token->value) == 0) {
      lexer->state = EXPR_CLASS;
    }
  }
  return token;
}

static const char *TypeString[] = {
  "None", "Integer", "Float", "Dot", "Identifier", "Space", "Keyword", "Newline",
  "Left Paren", "Right Paren", "Left Bracket", "Right Bracket", "Comma", "String Start", "String Content",
  "String End", "Left Brace", "Right Brace", "Symbol Beginning", "Colon 2",
  "BAR", "NOT", "Assignment", "NOT_EQUAL", "NOT_MATCH", "RIGHT_SHIFT", "OP_ASSIGN",
  "GREATER_THAN", "GREATER_THAN_OR_EQUAL", "COLON3", "tINSTANCE_VAR", "tCLASS_VAR", "tIGNORED_tNEWLINE",
  "CONSTANT", "tSEMICOLON", "tPLUS", "tUPLUS", "tMINUS", "tUMINUS", "tLAMBDA", "tTILDE", "tMULTIPLY",
  "tDIVIDE", "tPOW", "tREGEXP_BEG", "tPERCENT", "tCARET", "tAMPER", "tAND_OP", "tAND_DOT", "tOROP",
  "tLT", "tLEQ", "tLSHIFT", "tCMP", "tEQ", "tEQQ", "tMATCH", "tASSOC", "tFID"
};
static void print_token(Token* token) {
  printf("-- token %s '%s' at (%lu, %lu)\n", TypeString[token->type], token->value, token->lineno, token->start);
}

char peek() {
  return lexer->code[lexer->curr_pos+1];
}

void pushback() {
  lexer->in_token = false;
  lexer->curr_type = tNONE;
  lexer->curr_pos--;
}

bool valid_identifier_char() {
  //FIXME: use proper Ruby lexical rules for identifiers (start, mid and end)
  char next_char = peek();
  if (lexer->curr_char == '!' || lexer->curr_char == '?') {
    // e.g. name! or name?
    if (next_char != '=') {
      return true;
    }
  }
  return isalpha(lexer->curr_char);
}

bool is_after_operator() {
  return (lexer->state == EXPR_FNAME || lexer->state == EXPR_DOT) ? true : false;
}

bool is_expression_beginning() {
  return (lexer->state == EXPR_BEG || lexer->state == EXPR_MID || lexer->state == EXPR_CLASS) ? true : false;
}

/*
 * FIXME: This is a tempory solution to looking up keywords.
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
