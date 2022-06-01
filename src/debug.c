#ifdef DEBUG
  #include <stdbool.h>
  #include <stdio.h>
  
  #include "lexer.h"
  
  void print_tokens(lex_state* s) {
    for (long i = 0; i < s->tokens_amount; i++) {
      switch (s->tokens[i].type) {
        #define F(t) \
          case t: \
            fputs(#t, stdout); \
          break;
        ENUMERATE_TOKENS(F)
        #undef F
      }
      
      if (s->tokens[i].data) {
        printf("\t\t%s\n", s->tokens[i].data);
      } else {
        putchar('\n');
      }
    }
  }
#endif
