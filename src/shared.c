#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void* try(void* input) {
  if (input == NULL) {
    perror("hyperlisp");
    exit(1);
  } else {
    return input;
  }
}

// this is is a mini-lexer, ripping out some code from it to iterate over the file
// there may be a more libc-this way to do this
void print_error_and_exit(FILE* file, const char* filename, long cur_line, long cur_char, const char* string) {
  rewind(file);
  char ch = fgetc(file);
  
  for (int i = 0; i < cur_line; i++) {
    while (true) {
      switch (ch) {
        case '\n':
          goto break_loop_1;
        break;
        case '\r':
          ch = fgetc(file);
          if (ch == '\n') {
            goto break_loop_1;
          }
        break;
        case EOF:
          goto break_loop_1;
        break;
      }
      
      ch = fgetc(file);
    }
    break_loop_1:
    
    ch = fgetc(file);
  }
  
  while (true) {
    switch (ch) {
      case '\n':
        goto break_loop_2;
      break;
      case '\r':
        ch = fgetc(file);
        if (ch == '\n') {
          goto break_loop_2;
        }
      break;
      case EOF:
        goto break_loop_2;
      break;
    }
    
    fputc(ch, stderr);
    ch = fgetc(file);
  }
  break_loop_2:
  fputc('\n', stderr);
  
  char* char_arrow = try(malloc(cur_char+2));
  
  for (int i = 0; i < cur_char; i++) {
    char_arrow[i] = ' ';
  }
  char_arrow[cur_char] = '^';
  char_arrow[cur_char+1] = '\0';
  
  fprintf(stderr, "%s\n", char_arrow);
  free(char_arrow);
  
  fprintf(stderr, "%s:%li:%li  %s\n", filename, cur_line+1, cur_char+1, string);
  exit(1);
}
