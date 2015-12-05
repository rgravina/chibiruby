#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

void parse_program();
void parse_compound_statement();
void parse_statement();
void parse_expression();
void parse_terminal();
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
  // COMPSTMT: STMT (TERM EXPR)* [TERM]
  print_message("- non-terminal: compstmt");
  parse_statement();
  Token* curr_token = crb_curr_token();
  // // could be nothing, a terminal or several terminal and expressions
  while (curr_token != NULL) {
    parse_terminal();
    curr_token = crb_curr_token();
    if (curr_token != NULL) {
      parse_expression();
      curr_token = crb_curr_token();
    }
  }
}

void parse_statement() {
  print_message("- non-terminal: stmt");
  /*
  STMT: CALL do [`|' [BLOCK_VAR] `|'] COMPSTMT end
      | LHS `=' COMMAND [do [`|' [BLOCK_VAR] `|'] COMPSTMT end]
      | alias FNAME FNAME
      | undef (FNAME | SYMBOL)+
      | STMT if EXPR
      | STMT while EXPR
      | STMT unless EXPR
      | STMT until EXPR
      | STMT rescue STMT
      | `BEGIN' `{' COMPSTMT `}'
      | `END' `{' COMPSTMT `}'
      | EXPR
  */
  parse_expression();
}

void parse_expression() {
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
      crb_next_token();
      parse_command();
    }
    parse_arg();
}

void parse_terminal() {
    /*
      TERM: `;' | `\n'
    */
    Token* token = crb_curr_token();
    if (token->type == tSEMICOLON || token->type == tNEWLINE) {
      print_message("- Found terminal");
      crb_next_token();
    } else {
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
    /*
    ARG: LHS `=' ARG
      | LHS OP_ASGN ARG
      | ARG `..' ARG
      | ARG `...' ARG
      | ARG `+' ARG
      | ARG `-' ARG
      | ARG `*' ARG
      | ARG `/' ARG
      | ARG `%' ARG
      | ARG `**' ARG
      | `+' ARG
      | `-' ARG
      | ARG `|' ARG
      | ARG `^' ARG
      | ARG `&' ARG
      | ARG `<=>' ARG
      | ARG `>' ARG
      | ARG `>=' ARG
      | ARG `<' ARG
      | ARG `<=' ARG
      | ARG `==' ARG
      | ARG `===' ARG
      | ARG `!=' ARG
      | ARG `=~' ARG
      | ARG `!~' ARG
      | `!' ARG
      | `~' ARG
      | ARG `<<' ARG
      | ARG `>>' ARG
      | ARG `&&' ARG
      | ARG `||' ARG
      | defined? ARG
      | PRIMARY
  */
  Token* token = crb_curr_token();
  if (token == NULL) {
    return false;
  }
  switch (token->type) {
    case tNOT:
      print_message("- Found '!'");
      crb_next_token();
      parse_arg_dash();
      break;
    case tUPLUS:
      print_message("- Found unary '+'");
      crb_next_token();
      parse_arg_dash();
      break;
    case tUMINUS:
      print_message("- Found unary '-'");
      crb_next_token();
      parse_arg_dash();
      break;
    case tTILDE:
      print_message("- Found '~'");
      crb_next_token();
      parse_arg();
      break;
    case tKEYWORD:
      if (strcmp(token->value, "defined?") == 0) {
        print_message("- Found keyword 'defined?'");
        crb_next_token();
        parse_arg();
        parse_arg_dash();
      }
      break;
    default:
      // parse_arg_dash created to remove left-recursion of ARG op ARG statements.
      if (parse_lhs()) {
        crb_next_token();
        if (token->type == tIDENTIFIER) {
          parse_arg();
          parse_arg_dash();
        }
      } else {
        parse_primary() && parse_arg_dash();
      }
  }
  return true;
}

bool parse_arg_dash() {
  Token* token = crb_curr_token();
  if (token == NULL) {
    return false;
  }
  switch (token->type) {
    case tDOT2:
      print_message("- Found '..'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tDOT3:
      print_message("- Found '...'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tPLUS:
      print_message("- Found '+'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tMINUS:
      print_message("- Found '-'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tMULTIPLY:
      print_message("- Found '*'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tDIVIDE:
      print_message("- Found '/'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tPERCENT:
      print_message("- Found '%'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tOROP:
      print_message("- Found '||'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tPOW:
      print_message("- Found '**'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tBAR:
      print_message("- Found '|'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tCARET:
      print_message("- Found '^'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tAMPER:
      print_message("- Found '&'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tAND_OP:
      print_message("- Found '&&'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tAND_DOT:
      print_message("- Found '&.'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tLT:
      print_message("- Found '<'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tLEQ:
      print_message("- Found '<='");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tLSHIFT:
      print_message("- Found '<<'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tCMP:
      print_message("- Found '<=>'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tGT:
      print_message("- Found '>'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tGEQ:
      print_message("- Found '>='");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tRSHIFT:
      print_message("- Found '>>'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tEQ:
      print_message("- Found '=='");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tEQQ:
      print_message("- Found '==='");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tMATCH:
      print_message("- Found '=~'");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tNOT_EQUAL:
      print_message("- Found '!='");
      crb_next_token();
      parse_arg();
      parse_arg_dash();
      break;
    case tNOT_MATCH:
      print_message("- Found '!~'");
      crb_next_token();
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
  print_message("- non-terminal: primary");
  /*
  PRIMARY: `(' COMPSTMT `)'
    | LITERAL
    | VARIABLE
    | PRIMARY `::' identifier
    | `::' identifier
    | PRIMARY `[' [ARGS] `]'
    | `[' [ARGS [`,']] `]'
    | `{' [(ARGS|ASSOCS) [`,']] `}'
    | return [`(' [CALL_ARGS] `)']
    | yield [`(' [CALL_ARGS] `)']
    | defined? `(' ARG `)'
    | FUNCTION
    | FUNCTION `{' [`|' [BLOCK_VAR] `|'] COMPSTMT `}'
    | if EXPR THEN
      COMPSTMT
      (elsif EXPR THEN COMPSTMT)*
      [else COMPSTMT]
      end
    | unless EXPR THEN
      COMPSTMT
      [else COMPSTMT]
      end
    | while EXPR DO COMPSTMT end
    | until EXPR DO COMPSTMT end
    | case [EXPR]
      (when WHEN_ARGS THEN COMPSTMT)+
      [else COMPSTMT]
      end
    | for BLOCK_VAR in EXPR DO
      COMPSTMT
      end
    | begin
      COMPSTMT
      [rescue [ARGS] [`=>' LHS] THEN COMPSTMT]+
      [else COMPSTMT]
      [ensure COMPSTMT]
      end
    | class identifier [`<' identifier]
      COMPSTMT
      end
    | module identifier
      COMPSTMT
      end
    | def FNAME ARGDECL
      COMPSTMT
      [rescue [ARGS] [`=>' LHS] THEN COMPSTMT]+
      [else COMPSTMT]
      [ensure COMPSTMT]
      end
    | def SINGLETON (`.'|`::') FNAME ARGDECL
      COMPSTMT
      end
  */
  return parse_literal();
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
  print_message("- non-terminal: lhs");
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
      case tLBRACE:
        print_message("- terminal: '['");
        curr_token = crb_next_token();
        break;
      case tDOT:
        print_message("- terminal: '.'");
        curr_token = crb_next_token();
        if (curr_token->type == tIDENTIFIER) {
          if (parser->debug == true) {
            printf("- Found identifier: %s\n", curr_token->value);
          }
          crb_next_token();
        }
        break;
      default:
        // parse error or just lookahead couldn't match this, so go back.
        crb_set_token(snapshot);
        return false;
    }
  }
  return true;
}

bool parse_varname() {
  print_message("- non-terminal: varname");
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
      crb_next_token();
      break;
    case tINSTANCE_VAR:
      if (parser->debug == true) {
        printf("- Found instance var: %s\n", token->value);
      }
      crb_next_token();
      break;
    case tCLASS_VAR:
      if (parser->debug == true) {
        printf("- Found class var: %s\n", token->value);
      }
      crb_next_token();
      break;
    default:
      return false;
  }
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

void parse_args() {
  /*
  ARGS: ARG (`,' ARG)*
  */
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
  print_message("- non-terminal: variable");
  /*
  VARIABLE        : VARNAME
                  | self
                  | nil
                  | true
                  | false
                  | __FILE__
                  | __LINE__
  */
  return parse_varname();
}

bool parse_literal() {
  print_message("- non-terminal: literal");
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
      print_token(token);
      crb_next_token();
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

void print_token(Token* token) {
  if (parser->debug == true) {
    printf("- Found: %s\n", token->value);
  }
}

void print_message(char* string) {
  if (parser->debug == true) {
    puts(string);
  }
}
