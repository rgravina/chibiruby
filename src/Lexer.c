#include <stdio.h>
#include <string.h>
#include "lexer.h"

/* An integer */
struct tINTEGER {
  int lineno;
  char* value;
};

char* crb_lexer_lex(char* code) {
  //int len = strlen(code);
  //int current_pos = 0;
  // for (int i = 0; i < len; i++) {
  //   printf("%c", code[i]);
  // }
  return code;
}
