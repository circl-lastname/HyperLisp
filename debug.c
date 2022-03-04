#ifdef PRINT_TOKENS
  #include <stdio.h>
  
  #include "lexer.h"
  
  void print_tokens(lexstate* s) {
    for (long i = 0; i < s->tokens_amount; i++) {
      switch (s->tokens[i].type) {
        case BEGIN_BLOCK:
          fputs("BEGIN_BLOCK", stdout);
        break;
        case END_BLOCK:
          fputs("END_BLOCK", stdout);
        break;
        case BEGIN_LIST:
          fputs("BEGIN_LIST", stdout);
        break;
        case END_LIST:
          fputs("END_LIST", stdout);
        break;
        case BEGIN_VECTOR:
          fputs("BEGIN_VECTOR", stdout);
        break;
        case END_VECTOR:
          fputs("END_VECTOR", stdout);
        break;
        case SYMBOL:
          fputs("SYMBOL", stdout);
        break;
        case KEYWORD:
          fputs("KEYWORD", stdout);
        break;
        case LITERAL:
          fputs("LITERAL", stdout);
        break;
        case STRING:
          fputs("STRING", stdout);
        break;
        case END_TOKENS:
          fputs("END_TOKENS", stdout);
        break;
      }
      
      if (s->tokens[i].data) {
        printf("\t\t%s\n", s->tokens[i].data);
      } else {
        putchar('\n');
      }
    }
  }
#endif
