#include <stdio.h>
#include <stdlib.h>

// this is def code replication, but it works anywhere
void print_error_and_exit(FILE* file, const char* filename, long curline, long curchar, const char* string) {
  rewind(file);
  char ch = fgetc(file);
  
  for (int i = 0; i < curline; i++) {
    while (1) {
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
  
  while (1) {
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
    
    putchar(ch);
    ch = fgetc(file);
  }
  break_loop_2:
  putchar('\n');
  
  char* char_arrow = malloc(curchar+2);
  
  for (int i = 0; i < curchar; i++) {
    char_arrow[i] = ' ';
  }
  char_arrow[curchar] = '^';
  char_arrow[curchar+1] = '\0';
  
  fprintf(stderr, "%s\n", char_arrow);
  free(char_arrow);
  
  fprintf(stderr, "%s:%li:%li  %s\n", filename, curline+1, curchar+1, string);
  exit(1);
}
