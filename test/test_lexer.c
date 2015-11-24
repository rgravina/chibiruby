#include <stdio.h>
#include <string.h>
#include "minunit.h"
#include "lexer.h"

int tests_run = 0;
/*
require 'ripper'
require 'pp'
code = "10.times do |n|\n  puts n\nend"
pp Ripper.lex(code)
[[[1, 0], :on_int, "10"],
 [[1, 2], :on_period, "."],
 [[1, 3], :on_ident, "times"],
 [[1, 8], :on_sp, " "],
 [[1, 9], :on_kw, "do"],
 [[1, 11], :on_sp, " "],
 [[1, 12], :on_op, "|"],
 [[1, 13], :on_ident, "n"],
 [[1, 14], :on_op, "|"],
 [[1, 15], :on_ignored_nl, "\n"],
 [[2, 0], :on_sp, "  "],
 [[2, 2], :on_ident, "puts"],
 [[2, 6], :on_sp, " "],
 [[2, 7], :on_ident, "n"],
 [[2, 8], :on_nl, "\n"],
 [[3, 0], :on_kw, "end"]]
*/
static char *test_lexer_method_block() {
  char* code = "10.times do |n|\n  puts n\nend";
  Token* token;

  crb_init_lexer();
  crb_lexer_lex(code);
  mu_assert(lexer->num_tokens == 16, "token count incorrect");
  mu_assert(lexer->in_token == false, "in_token flag not set to false after processing token");

  token = lexer->head;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 0, "token start incorrect");
  mu_assert(token->type == INTEGER, "token type not set correctly");
  mu_assert(strcmp(token->value, "10") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 2, "token start incorrect");
  mu_assert(token->type == PERIOD, "token type not set correctly");
  mu_assert(strcmp(token->value, ".") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 3, "token start incorrect");
  mu_assert(token->type == IDENTIFIER, "token type not set correctly");
  mu_assert(strcmp(token->value, "times") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 8, "token start incorrect");
  mu_assert(token->type == SPACE, "token type not set correctly");
  mu_assert(strcmp(token->value, " ") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 9, "token start incorrect");
  mu_assert(token->type == KEYWORD, "token type not set correctly");
  mu_assert(strcmp(token->value, "do") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 11, "token start incorrect");
  mu_assert(token->type == SPACE, "token type not set correctly");
  mu_assert(strcmp(token->value, " ") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 12, "token start incorrect");
  mu_assert(token->type == OPERATOR, "token type not 'operator'");
  mu_assert(strcmp(token->value, "|") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 13, "token start incorrect");
  mu_assert(token->type == IDENTIFIER, "token type not 'identifier'");
  mu_assert(strcmp(token->value, "n") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 14, "token start incorrect");
  mu_assert(token->type == OPERATOR, "token type not 'operator'");
  mu_assert(strcmp(token->value, "|") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 15, "token start incorrect");
  mu_assert(token->type == NEWLINE, "token type not set correctly");
  mu_assert(strcmp(token->value, "\n") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 2, "token line number incorrect");
  mu_assert(token->start == 0, "token start incorrect");
  mu_assert(token->type == SPACE, "token type not set correctly");
  mu_assert(strcmp(token->value, "  ") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 2, "token line number incorrect");
  mu_assert(token->start == 2, "token start incorrect");
  mu_assert(token->type == IDENTIFIER, "token type not 'identifier'");
  mu_assert(strcmp(token->value, "puts") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 2, "token line number incorrect");
  mu_assert(token->start == 6, "token start incorrect");
  mu_assert(token->type == SPACE, "token type not set correctly");
  mu_assert(strcmp(token->value, " ") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 2, "token line number incorrect");
  mu_assert(token->start == 7, "token start incorrect");
  mu_assert(token->type == IDENTIFIER, "token type not 'identifier'");
  mu_assert(strcmp(token->value, "n") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 2, "token line number incorrect");
  mu_assert(token->start == 8, "token start incorrect");
  mu_assert(token->type == NEWLINE, "token type not set correctly");
  mu_assert(strcmp(token->value, "\n") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 3, "token line number incorrect");
  mu_assert(token->start == 0, "token start incorrect");
  mu_assert(token->type == KEYWORD, "token type not set correctly");
  mu_assert(strcmp(token->value, "end") == 0, "token not parsed correctly");

  crb_free_lexer();
  return 0;
}

/*
require 'ripper'
require 'pp'
code = "array = [1,2,3]\n puts array[1]"
pp Ripper.lex(code)
[[[1, 0], :on_ident, "array"],
 [[1, 5], :on_sp, " "],
 [[1, 6], :on_op, "="],
 [[1, 7], :on_sp, " "],
 [[1, 8], :on_lbracket, "["],
 [[1, 9], :on_int, "1"],
 [[1, 10], :on_comma, ","],
 [[1, 11], :on_int, "2"],
 [[1, 12], :on_comma, ","],
 [[1, 13], :on_int, "3"],
 [[1, 14], :on_rbracket, "]"],
 [[1, 15], :on_nl, "\n"],
 [[2, 0], :on_sp, " "],
 [[2, 1], :on_ident, "puts"],
 [[2, 5], :on_sp, " "],
 [[2, 6], :on_ident, "array"],
 [[2, 11], :on_lbracket, "["],
 [[2, 12], :on_int, "1"],
 [[2, 13], :on_rbracket, "]"]]
*/
static char *test_lexer_array() {
  char* code = "array = [1,2,3]\n puts array[1]";
  Token* token;

  crb_init_lexer();
  crb_lexer_lex(code);
  mu_assert(lexer->num_tokens == 19, "token count incorrect");
  mu_assert(lexer->in_token == false, "in_token flag not set to false after processing token");

  token = lexer->head;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 0, "token start incorrect");
  mu_assert(token->type == IDENTIFIER, "token type not set correctly");
  mu_assert(strcmp(token->value, "array") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 5, "token start incorrect");
  mu_assert(token->type == SPACE, "token type not set correctly");
  mu_assert(strcmp(token->value, " ") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 6, "token start incorrect");
  mu_assert(token->type == OPERATOR, "token type not set correctly");
  mu_assert(strcmp(token->value, "=") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 7, "token start incorrect");
  mu_assert(token->type == SPACE, "token type not set correctly");
  mu_assert(strcmp(token->value, " ") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 8, "token start incorrect");
  mu_assert(token->type == LBRACKET, "token type not set correctly");
  mu_assert(strcmp(token->value, "[") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 9, "token start incorrect");
  mu_assert(token->type == INTEGER, "token type not set correctly");
  mu_assert(strcmp(token->value, "1") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 10, "token start incorrect");
  mu_assert(token->type == COMMA, "token type not set correctly");
  mu_assert(strcmp(token->value, ",") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 11, "token start incorrect");
  mu_assert(token->type == INTEGER, "token type not set correctly");
  mu_assert(strcmp(token->value, "2") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 12, "token start incorrect");
  mu_assert(token->type == COMMA, "token type not set correctly");
  mu_assert(strcmp(token->value, ",") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 13, "token start incorrect");
  mu_assert(token->type == INTEGER, "token type not set correctly");
  mu_assert(strcmp(token->value, "3") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 14, "token start incorrect");
  mu_assert(token->type == RBRACKET, "token type not set correctly");
  mu_assert(strcmp(token->value, "]") == 0, "token not parsed correctly");

  token = token->next;
  mu_assert(token->lineno == 1, "token line number incorrect");
  mu_assert(token->start == 15, "token start incorrect");
  mu_assert(token->type == NEWLINE, "token type not set correctly");
  mu_assert(strcmp(token->value, "\n") == 0, "token not parsed correctly");
  
  token = token->next;
  mu_assert(token->lineno == 2, "token line number incorrect");
  mu_assert(token->start == 0, "token start incorrect");
  mu_assert(token->type == SPACE, "token type not set correctly");
  mu_assert(strcmp(token->value, " ") == 0, "token not parsed correctly");
  
  token = token->next;
  mu_assert(token->lineno == 2, "token line number incorrect");
  mu_assert(token->start == 1, "token start incorrect");
  mu_assert(token->type == IDENTIFIER, "token type not 'identifier'");
  mu_assert(strcmp(token->value, "puts") == 0, "token not parsed correctly");
  
  token = token->next;
  mu_assert(token->lineno == 2, "token line number incorrect");
  mu_assert(token->start == 5, "token start incorrect");
  mu_assert(token->type == SPACE, "token type not set correctly");
  mu_assert(strcmp(token->value, " ") == 0, "token not parsed correctly");
  
  token = token->next;
  mu_assert(token->lineno == 2, "token line number incorrect");
  mu_assert(token->start == 6, "token start incorrect");
  mu_assert(token->type == IDENTIFIER, "token type not 'identifier'");
  mu_assert(strcmp(token->value, "array") == 0, "token not parsed correctly");

  crb_free_lexer();
  return 0;
}

// TODO: more cases
/*
code = "`cvs diff parse.y`"
> pp Ripper.lex(code)
[[[1, 0], :on_backtick, "`"],
 [[1, 1], :on_tstring_content, "cvs diff parse.y"],
 [[1, 17], :on_tstring_end, "`"]]

code = 'obj.`("cvs diff parse.y")'
> pp Ripper.lex(code)
[[[1, 0], :on_ident, "obj"],
 [[1, 3], :on_period, "."],
 [[1, 4], :on_backtick, "`"],
 [[1, 5], :on_lparen, "("],
 [[1, 6], :on_tstring_beg, "\""],
 [[1, 7], :on_tstring_content, "cvs diff parse.y"],
 [[1, 23], :on_tstring_end, "\""],
 [[1, 24], :on_rparen, ")"]]
 
code = "print(<<EOS)"
> pp Ripper.lex(code)
[[[1, 0], :on_ident, "print"],
[[1, 5], :on_lparen, "("],
[[1, 6], :on_heredoc_beg, "<<EOS"]]

code = "class Greeter\n   def initialize(name = \"World\")\n     @name = name\n   end\n\n   def say_hi\n     puts \"Hi #{@name}!\"\n   end\n   def say_bye\n     puts \"Bye #{@name}, come back soon.\"\n   end\n end"
> pp Ripper.lex(code)
[[[1, 0], :on_kw, "class"],
 [[1, 5], :on_sp, " "],
 [[1, 6], :on_const, "Greeter"],
 [[1, 13], :on_nl, "\n"],
 [[2, 0], :on_sp, "   "],
 [[2, 3], :on_kw, "def"],
 [[2, 6], :on_sp, " "],
 [[2, 7], :on_ident, "initialize"],
 [[2, 17], :on_lparen, "("],
 [[2, 18], :on_ident, "name"],
 [[2, 22], :on_sp, " "],
 [[2, 23], :on_op, "="],
 [[2, 24], :on_sp, " "],
 [[2, 25], :on_tstring_beg, "\""],
 [[2, 26], :on_tstring_content, "World"],
 [[2, 31], :on_tstring_end, "\""],
 [[2, 32], :on_rparen, ")"],
 [[2, 33], :on_ignored_nl, "\n"],
 [[3, 0], :on_sp, "     "],
 [[3, 5], :on_ivar, "@name"],
 [[3, 10], :on_sp, " "],
 [[3, 11], :on_op, "="],
 [[3, 12], :on_sp, " "],
 [[3, 13], :on_ident, "name"],
 [[3, 17], :on_nl, "\n"],
 [[4, 0], :on_sp, "   "],
 [[4, 3], :on_kw, "end"],
 [[4, 6], :on_nl, "\n"],
 [[5, 0], :on_ignored_nl, "\n"],
 [[6, 0], :on_sp, "   "],
 [[6, 3], :on_kw, "def"],
 [[6, 6], :on_sp, " "],
 [[6, 7], :on_ident, "say_hi"],
 [[6, 13], :on_nl, "\n"],
 [[7, 0], :on_sp, "     "],
 [[7, 5], :on_ident, "puts"],
 [[7, 9], :on_sp, " "],
 [[7, 10], :on_tstring_beg, "\""],
 [[7, 11], :on_tstring_content, "Hi !"],
 [[7, 15], :on_tstring_end, "\""],
 [[7, 16], :on_nl, "\n"],
 [[8, 0], :on_sp, "   "],
 [[8, 3], :on_kw, "end"],
 [[8, 6], :on_nl, "\n"],
 [[9, 0], :on_sp, "   "],
 [[9, 3], :on_kw, "def"],
 [[9, 6], :on_sp, " "],
 [[9, 7], :on_ident, "say_bye"],
 [[9, 14], :on_nl, "\n"],
 [[10, 0], :on_sp, "     "],
 [[10, 5], :on_ident, "puts"],
 [[10, 9], :on_sp, " "],
 [[10, 10], :on_tstring_beg, "\""],
 [[10, 11], :on_tstring_content, "Bye , come back soon."],
 [[10, 32], :on_tstring_end, "\""],
 [[10, 33], :on_nl, "\n"],
 [[11, 0], :on_sp, "   "],
 [[11, 3], :on_kw, "end"],
 [[11, 6], :on_nl, "\n"],
 [[12, 0], :on_sp, " "],
 [[12, 1], :on_kw, "end"]]
 */

static char *all_tests() {
  mu_run_test(test_lexer_method_block);
  mu_run_test(test_lexer_array);
  return 0;
}

int main() {
  char *result = all_tests();
  if (result != 0) {
    printf("%s\n", result);
  }
  else {
    printf("ALL TESTS PASSED\n");
  }
  printf("Tests run: %d\n", tests_run);
  return result != 0;
}
