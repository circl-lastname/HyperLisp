#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "shared.h"
#include "lexer.h"
#include "parser.h"
#include "debug.h"

void main(int argc, char** argv) {
  if (argc != 3) {
    puts("usage: hyperlisp <input> <output>");
    exit(1);
  }
  
  FILE* file_in = try(fopen(argv[1], "rb"));
  FILE* file_out = try(fopen(argv[2], "wb"));
  
  lex_state lexs;
  
  lexs.file = file_in;
  lexs.filename = argv[1];
  lexs.ch = fgetc(file_in);
  lexs.after_newline = false;
  lexs.cur_line = 0;
  lexs.cur_char = 0;
  lexs.token_cur_line = 0;
  lexs.token_cur_char = 0;
  lexs.counter_disabled = false;
  lexs.tokens_amount = 0;
  lexs.tokens_size = 256;
  lexs.tokens = try(malloc(256*sizeof(lex_token)));
  
  lex(&lexs);
  
  #ifdef DEBUG
    print_tokens(&lexs);
  #endif
  
  prs_state prss;
  
  prss.file = file_out;
  prss.file_in = file_in;
  prss.filename_in = argv[1];
  prss.tk = &lexs.tokens[0];
  prss.current_token = 0;
  prss.put_doctype = false;
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
