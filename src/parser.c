#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"

void parse_program();
void parse_compound_statement(Token* token);
void parse_statement(Token* token);
void parse_expression(Token* token);
void parse_terminal(Token* token);
void parse_literal(Token* token);
void parse_primary(Token* token);
void parse_arg(Token* token);
void parse_command(Token* token);
void parse_lhs(Token* token);
void parse_varname(Token* token);

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
  Token* token = crb_next_token();
  parse_compound_statement(token);
}

void parse_compound_statement(Token* token) {
  // COMPSTMT: STMT (TERM EXPR)* [TERM]
  parse_statement(token);
  Token* next_token = crb_next_token();
  // could be nothing, a terminal or several terminal and expressions
  while (next_token != NULL) {
    parse_terminal(next_token);
    next_token = crb_next_token();
    if (next_token != NULL) {
      parse_expression(next_token);
    }
  }
}

void parse_statement(Token* token) {
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
  parse_expression(token);
}

void parse_expression(Token* token) {
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
    if (token->type == tNOT) {
      puts("- Found '!'");
      parse_command(crb_next_token());
    }
    parse_arg(token);
}

void parse_terminal(Token* token) {
    /*
      TERM: `;' | `\n'
    */
    if (token->type == tSEMICOLON || token->type == tNEWLINE) {
      if (parser->debug == true) {
        puts("- Found terminal");
      }
    } else {
    }
}

void parse_call() {
  /*
  CALL: FUNCTION
      | COMMAND
  */
}

void parse_command(Token* token) {
    /*
    COMMAND: OPERATION CALL_ARGS
          | PRIMARY `.' FNAME CALL_ARGS
          | PRIMARY `::' FNAME CALL_ARGS
          | super CALL_ARGS
          | yield CALL_ARGS
    */
    parse_primary(token);
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

void parse_arg(Token* token) {
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
  switch (token->type) {
    case tNOT:
      puts("- Found '!'");
      parse_arg(crb_next_token());
      break;
    case tUPLUS:
      puts("- Found unary '+'");
      parse_arg(crb_next_token());
      break;
    default:
      parse_lhs(token);
      parse_primary(token);
  }
}

void parse_primary(Token* token) {
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
  parse_literal(token);
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

void parse_lhs(Token* token) {
  /*
  LHS: VARNAME
      | PRIMARY `[' [ARGS] `]'
      | PRIMARY `.' identifier
  */
  parse_varname(token);
}

void parse_varname(Token* token) {
  /*
  VARNAME: GLOBAL
    | `@'identifier
    | `@@'identifier
    | identifier
  */
  switch (token->type) {
    case tIDENTIFIER:
      if (parser->debug == true) {
        printf("- Found identifier: %s\n", token->value);
      }
      break;
    case tINSTANCE_VAR:
      if (parser->debug == true) {
        printf("- Found instance var: %s\n", token->value);
      }
      break;
    case tCLASS_VAR:
      if (parser->debug == true) {
        printf("- Found class var: %s\n", token->value);
      }
      break;
    default:
      break;
  }
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

void parse_variable() {
  /*
  VARIABLE        : VARNAME
                  | self
                  | nil
                  | true
                  | false
                  | __FILE__
                  | __LINE__
  */
}

void parse_literal(Token* token) {
  /*
  LITERAL         : numeric
                  | SYMBOL
                  | STRING
                  | HERE_DOC
                  | WORDS
                  | REGEXP
  */
  if (token->type == tINTEGER) {
    if (parser->debug == true) {
      printf("- Found integer literal: %s\n", token->value);
    }
  } else {
  }
}

void parse_string() {
  /*
  STRING          : LITERAL_STRING+
  */
}

void parse_term() {
  /*
  TERM            : `;'
                  | `\n'
  */
}
