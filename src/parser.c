#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

void parse_program();
void parse_compound_statement();
bool parse_statement();
bool parse_expression();
bool parse_terminal();
bool parse_literal();
bool parse_primary();
bool parse_arg();
bool parse_arg_dash();
void parse_command();
bool parse_lhs();
bool parse_varname();
void parse_string();
void parse_symbol();
void print_token();
bool parse_variable();
bool parse_args();
Token* next();

void print_message(char* string);

void crb_init_parser(char* code) {
  crb_init_lexer(code);
  parser = (Parser*)malloc(sizeof(Parser));
  parser->lexer = lexer;
  parser->debug = false;
}

/*
 * Parser initialise/free function
 */
void crb_parser_parse() {
  crb_lexer_lex();
  parse_program();
}

void crb_free_parser() {
  crb_free_lexer();
  free(parser);
}

/*
 * Grammar parsing functions
 */

 /*
  * Program structure
  */
void parse_program() {
  // PROGRAM: COMPSTMT
  print_message("- non-terminal: program");
  crb_next_token();
  parse_compound_statement();
}

void parse_compound_statement() {
  print_message("- non-terminal: compstmt");
  Token* token = crb_curr_token();
  while (token != NULL && parse_statement()) {
    token = crb_curr_token();
    if (token != NULL) {
      parse_terminal();
    }
    token = crb_curr_token();
  }
}

bool parse_statement() {
  print_message("- non-terminal: stmt");
  return parse_expression();
}

bool parse_expression() {
    print_message("- non-terminal: expr");
    /*
    EXPR: MLHS `=' MRHS
        | return CALL_ARGS
        | EXPR and EXPR
        | EXPR or EXPR
        | not EXPR
        | COMMAND
        | `!' COMMAND
        | ARG
    */
    Token* token = crb_curr_token();
    if (token->type == tNOT) {
      print_message("- Found '!'");
      next();
      parse_command();
      return true;
    } else {
      return parse_arg();
    }
  return false;
}

bool parse_terminal() {
    /*
      TERM: `;' | `\n'
    */
    Token* token = crb_curr_token();
    if (token->type == tSEMICOLON || token->type == tNEWLINE) {
      print_message("- Found terminal");
      next();
      return true;
    } else {
      return false;
    }
}

void parse_call() {
  /*
  CALL: FUNCTION
      | COMMAND
  */
}

void parse_command() {
    print_message("- non-terminal: command");
    /*
    COMMAND: OPERATION CALL_ARGS
          | PRIMARY `.' FNAME CALL_ARGS
          | PRIMARY `::' FNAME CALL_ARGS
          | super CALL_ARGS
          | yield CALL_ARGS
    */
    parse_primary();
}

void parse_function() {
    /*
    FUNCTION: OPERATION [`(' [CALL_ARGS] `)']
      | PRIMARY `.' FNAME `(' [CALL_ARGS] `)'
      | PRIMARY `::' FNAME `(' [CALL_ARGS] `)'
      | PRIMARY `.' FNAME
      | PRIMARY `::' FNAME
      | super [`(' [CALL_ARGS] `)']
    */
}

bool parse_arg() {
  print_message("- non-terminal: arg");
  Token* token = crb_curr_token();
  if (token == NULL) {
    return false;
  }
  Token* snapshot = crb_curr_token();
  switch (token->type) {
    case tNOT:
      print_message("- Found '!'");
      next();
      parse_arg_dash();
      return true;
      break;
    case tUPLUS:
      print_message("- Found unary '+'");
      next();
      parse_arg_dash();
      return true;
      break;
    case tUMINUS:
      print_message("- Found unary '-'");
      next();
      parse_arg_dash();
      return true;
      break;
    case tTILDE:
      print_message("- Found '~'");
      next();
      parse_arg();
      return true;
      break;
    case tKEYWORD:
      if (strcmp(token->value, "defined?") == 0) {
        print_message("- Found keyword 'defined?'");
        next();
        parse_arg();
        parse_arg_dash();
        return true;
      }
      break;
    default:
      break;
  }
  // try lhs = arg arg'
  if (parse_lhs()) {
    token = crb_curr_token();
    if (token == NULL) {
      print_message("- rolling back non terminal - matches part of LHS = ARG ARG'");
      crb_set_token(snapshot);
    } else if (token->type == tASSIGN) {
      print_message("- Found '='");
      next();
      parse_arg();
      parse_arg_dash();
      return true;
    } else {
      print_message("- rolling back non terminal - matches part of LHS = ARG ARG'");
      crb_set_token(snapshot);
    }
  }
  // if gets to here, was not lhs = arg arg'. Try this alternative.
  return parse_primary() && parse_arg_dash();
}

// parse_arg_dash created to remove left-recursion of ARG op ARG statements.
bool parse_arg_dash() {
  Token* token = crb_curr_token();
  if (token == NULL) {
    return false;
  }
  switch (token->type) {
    case tDOT2:
      print_message("- Found '..'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tDOT3:
      print_message("- Found '...'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tPLUS:
      print_message("- Found '+'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tMINUS:
      print_message("- Found '-'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tMULTIPLY:
      print_message("- Found '*'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tDIVIDE:
      print_message("- Found '/'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tPERCENT:
      print_message("- Found '%'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tOROP:
      print_message("- Found '||'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tPOW:
      print_message("- Found '**'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tBAR:
      print_message("- Found '|'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tCARET:
      print_message("- Found '^'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tAMPER:
      print_message("- Found '&'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tAND_OP:
      print_message("- Found '&&'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tAND_DOT:
      print_message("- Found '&.'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tLT:
      print_message("- Found '<'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tLEQ:
      print_message("- Found '<='");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tLSHIFT:
      print_message("- Found '<<'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tCMP:
      print_message("- Found '<=>'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tGT:
      print_message("- Found '>'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tGEQ:
      print_message("- Found '>='");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tRSHIFT:
      print_message("- Found '>>'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tEQ:
      print_message("- Found '=='");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tEQQ:
      print_message("- Found '==='");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tMATCH:
      print_message("- Found '=~'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tNOT_EQUAL:
      print_message("- Found '!='");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    case tNOT_MATCH:
      print_message("- Found '!~'");
      next();
      parse_arg();
      parse_arg_dash();
      break;
    default:
      // empty production is OK
      break;
  }
  return true;
}

bool parse_primary() {
  bool result = parse_literal() || parse_varname();
  if (!result) {
    Token* token = crb_curr_token();
    if (token == NULL) {
      return false;
    }
    switch(token->type) {
    case tLPAREN:
      print_message("- Found '('");
      next();
      parse_compound_statement();
      if (token->type == tRPAREN) {
        print_message("- Found ')'");
        next();
        result = true;
      }
      break;
    case tCOLON3:
      print_message("- Found '::' tCOLON3 e.g. ::a");
      next();
      if (token->type == tIDENTIFIER) {
        print_message("- Found identifier");
        next();
        result = true;
      }
      break;
    case tKEYWORD:
      if (strcmp(token->value, "class") == 0) {
        print_message("- Found 'class'");
        token = next();
        if (token->type == tCONSTANT) {
          print_message("- Found constant");
          token = next();
        }
        if (token->type == tLT) {
          print_message("- Found '<'");
          token = next();
          if (token->type == tCONSTANT) {
            print_message("- Found constant");
            token = next();
          }
        }
        token = next();
        parse_compound_statement();
        token = crb_curr_token();
        if (token->type == tKEYWORD && strcmp(token->value, "end") == 0) {
          print_message("- Found end");
          token = next();
        }
        result = true;
      }
      break;
    default:
      break;
    }
  }
  if (result)
    print_message("- non-terminal: primary");
  return result;
}

void parse_when_args() {
    /*
    WHEN_ARGS: ARGS [`,' `*' ARG]
      | `*' ARG
    */
}

void parse_then() {
  /*
  THEN: TERM
      | then
      | TERM then
  */
}

void parse_do() {
  /*
  DO: TERM
    | do
    | TERM do
  */
}

void parse_block_var() {
  /*
  BLOCK_VAR: LHS
          | MLHS
  */
}

void parse_mlhs() {
  /*
  MLHS: MLHS_ITEM `,' MLHS_ITEM [(`,' MLHS_ITEM)*] [`,' `*' [LHS]]
      | MLHS_ITEM `,' `*' [LHS]
      | MLHS_ITEM [(`,' MLHS_ITEM)*] `,'
      | `*' [LHS]
      | `(' MLHS `)'
  */
}

void parse_mlhs_item() {
  /*
  MLHS_ITEM: LHS
          | '(' MLHS ')'
  */
}

bool parse_lhs() {
  /*
  LHS: VARNAME
      | PRIMARY `[' [ARGS] `]'
      | PRIMARY `.' identifier
  */
  if (!parse_varname()) {
    Token* snapshot = crb_curr_token();
    parse_primary();
    Token* curr_token = crb_curr_token();
    if (curr_token == NULL) {
      return false;
    }
    switch(curr_token->type) {
      case tLBRACKET:
        print_message("- terminal: '['");
        curr_token = next();
        if (curr_token->type == tRBRACKET) {
          print_message("- terminal: ']'");
          curr_token = next();
          break;
        }
        parse_args();
        if (curr_token->type == tRBRACKET) {
          print_message("- terminal: ']'");
          curr_token = next();
        }
        break;
      case tDOT:
        print_message("- terminal: '.'");
        curr_token = next();
        if (curr_token->type == tIDENTIFIER) {
          if (parser->debug == true) {
            printf("- Found identifier: %s\n", curr_token->value);
          }
          next();
        }
        break;
      default:
        // parse error or just lookahead couldn't match this, so go back.
        print_message("- rolling back non terminal - matches part of LHS: primary '[' or primary '.'");
        crb_set_token(snapshot);
        return false;
    }
  }
  print_message("- non-terminal: lhs");
  return true;
}

bool parse_varname() {
  /*
  VARNAME: GLOBAL
    | `@'identifier
    | `@@'identifier
    | identifier
  */
  Token* token = crb_curr_token();
  if (token == NULL) {
    return false;
  }
  switch (token->type) {
    case tIDENTIFIER:
      if (parser->debug == true) {
        printf("- Found identifier: %s\n", token->value);
      }
      next();
      break;
    case tINSTANCE_VAR:
      if (parser->debug == true) {
        printf("- Found instance var: %s\n", token->value);
      }
      next();
      break;
    case tCLASS_VAR:
      if (parser->debug == true) {
        printf("- Found class var: %s\n", token->value);
      }
      next();
      break;
    default:
      return false;
  }
  print_message("- non-terminal: varname");
  return true;
}

void parse_mrhs() {
  /*
  MRHS: ARGS [`,' `*' ARG]
      | `*' ARG
  */
}

void parse_call_args() {
  /*
  CALL_ARGS: ARGS
      | ARGS [`,' ASSOCS] [`,' `*' ARG] [`,' `&' ARG]
      | ASSOCS [`,' `*' ARG] [`,' `&' ARG]
      | `*' ARG [`,' `&' ARG]
      | `&' ARG
      | COMMAND
  */
}

bool parse_args() {
  /*
  ARGS: ARG (`,' ARG)*
  */
  if (parse_arg()) {
    puts("looks like parse args");
    Token* token = crb_curr_token();
    while (token->type == tCOMMA) {
      print_message("- terminal: ','");
      next();
      parse_arg();
    }
  } else {
    return false;
  }
  return true;
}

void parse_argdecl() {
  /*
  ARGDECL: `(' ARGLIST `)'
        | ARGLIST TERM
  */
}

void parse_arglist() {
  /*
  ARGLIST: identifier(`,'identifier)*[`,'`*'[identifier]][`,'`&'identifier]
        | `*'identifier[`,'`&'identifier]
        | [`&'identifier]
  */
}

void parse_singleton() {
  /*
  SINGLETON       : VARNAME
                  | self
                  | nil
                  | true
                  | false
                  | `(' EXPR `)'
  */
}

void parse_assocs() {
  /*
  ASSOCS          : ASSOC (`,' ASSOC)*
  */
}

void parse_assoc() {
  /*
  ASSOC           : ARG `=>' ARG
  */
}

bool parse_variable() {
  /*
  VARIABLE        : VARNAME
                  | self
                  | nil
                  | true
                  | false
                  | __FILE__
                  | __LINE__
  */
  bool result = parse_varname();
  if (result)
    print_message("- non-terminal: variable");
  return result;
}

bool parse_literal() {
  /*
  LITERAL         : numeric
                  | SYMBOL
                  | STRING
                  | HERE_DOC
                  | WORDS
                  | REGEXP
  */
  Token* token = crb_curr_token();
  if (token == NULL) {
    return false;
  }
  switch (token->type) {
    case tINTEGER:
      print_message("- non-terminal: literal");
      print_token(token);
      next();
      break;
    default:
      return false;
  }
  return true;
}

void parse_symbol() {
    /*
      SYMBOL: `:'FNAME
        | `:'`@'identifier
        | `:'`@@'identifier
        | `:'GLOBAL
    */
}

void parse_string() {
  /*
  STRING          : LITERAL_STRING+
  */
}

Token* next() {
  Token* token = crb_next_token();
  if (token == NULL) {
    return NULL;
  }
  while (token->type == tSPACE || token->type == tIGNORED_NEWLINE) {
    token = crb_next_token();
  }
  return crb_curr_token();
}


void print_token(Token* token) {
  if (parser->debug == true) {
    if (token != NULL) {
      printf("- token->value: %s\n", token->value);
    } else {
      puts("Token is NULL.");
    }
  }
}

void print_message(char* string) {
  if (parser->debug == true) {
    puts(string);
  }
}
