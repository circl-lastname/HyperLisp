#include <stdio.h>
#include <stdlib.h>

#include "shared.h"
#include "lexer.h"
#include "parser.h"
#include "print_tokens.h"

void main(const int argc, const char* argv[]) {
  if (argc != 3) {
    puts("usage: hyperlisp <input> <output>");
    exit(1);
  }
  
  FILE* file_in = try(fopen(argv[1], "rb"));
  FILE* file_out = try(fopen(argv[2], "wb"));
  
  lexstate lexs;
  
  lexs.file = file_in;
  lexs.filename = argv[1];
  lexs.ch = fgetc(file_in);
  lexs.after_newline = 0;
  lexs.curline = 0;
  lexs.curchar = 0;
  lexs.token_curline = 0;
  lexs.token_curchar = 0;
  lexs.counter_disabled = 0;
  lexs.tokens_amount = 0;
  lexs.tokens_size = 256;
  lexs.tokens = try(malloc(256*sizeof(lextoken)));
  
  lex(&lexs);
  
  #ifdef PRINT_TOKENS
    print_tokens(&lexs);
  #endif
  
  prsstate prss;
  
  prss.file = file_out;
  prss.file_in = file_in;
  prss.filename_in = argv[1];
  prss.tk = &lexs.tokens[0];
  prss.current_token = 0;
  prss.put_doctype = 0;
  prss.tokens_amount = lexs.tokens_amount;
  prss.tokens = lexs.tokens;
  
  parse(&prss);
  
  fclose(file_in);
  fclose(file_out);
  
  for (long i = 0; i < lexs.tokens_amount; i++) {
    if (lexs.tokens[i].data) {
      free(lexs.tokens[i].data);
    }
  }
  
  free(lexs.tokens);
}
