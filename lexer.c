#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

static void put_token(lexstate* s, lextype type, char* data) {
  lextoken* token = &s->tokens[s->tokens_amount];
  
  token->type = type;
  token->data = data;
  token->curline = s->curline;
  token->curchar = s->curchar;
  
  s->tokens_amount++;
  
  if (s->tokens_amount >= s->tokens_size) {
    s->tokens_size *= 2;
    s->tokens = realloc(s->tokens, s->tokens_size*sizeof(lextoken));
  }
}

static void consume(lexstate* s) {
  s->ch = fgetc(s->file);
  
  // this is a big if block to run every character, but
  // - it can't run during printing an error
  // - it needs to mark the next character after a newline for reset
  // - it can't update on an EOF, since it can result in unhelpful errors
  if (!s->in_error && s->ch != EOF) {
    if (s->after_newline) {
      s->after_newline = 0;
      s->curline++;
      s->curchar = 0;
    } else {
      s->curchar++;
      if (s->ch == '\n') {
        s->after_newline = 1;
      }
    }
  }
}

static void skip_until_newline(lexstate* s) {
  while (1) {
    consume(s);
    
    switch (s->ch) {
      case '\n':
        goto break_loop;
      break;
      case '\r':
        consume(s);
        if (s->ch == '\n') {
          goto break_loop;
        }
      break;
    }
  }
  break_loop:
  
  consume(s);
}

static char* get_until_newline(lexstate* s) {
  char* string = malloc(256);
  size_t string_size = 256;
  
  size_t i = 0;
  while (1) {
    switch (s->ch) {
      case '\n':
        goto break_loop;
      break;
      case '\r':
        consume(s);
        if (s->ch == '\n') {
          goto break_loop;
        }
      break;
      case EOF:
        goto break_loop;
      break;
    }
    
    string[i] = s->ch;
    i++;
    consume(s);
    
    if (i >= string_size) {
      string_size *= 2;
      string = realloc(string, string_size);
    }
  }
  break_loop:
  
  string[i] = '\0';
  
  return string;
}

static void error(lexstate* s, const char* string) {
  s->in_error = 1;
  
  rewind(s->file);
  consume(s);
  for (int i = 0; i < s->curline; i++) {
    skip_until_newline(s);
  }
  
  char* line = get_until_newline(s);
  fprintf(stderr, "%s\n", line);
  free(line);
  
  char* char_arrow = malloc(s->curchar+2);
  
  for (int i = 0; i <= s->curchar; i++) {
    char_arrow[i] = ' ';
  }
  char_arrow[s->curchar] = '^';
  char_arrow[s->curchar+1] = '\0';
  
  fprintf(stderr, "%s\n", char_arrow);
  free(char_arrow);
  
  fprintf(stderr, "%s:%i:%i  %s\n", s->filename, s->curline+1, s->curchar+1, string);
  exit(1);
}

static void skip_space(lexstate* s) {
  while (isspace(s->ch)) {
    consume(s);
  }
}

static char* get_until_disallowed(lexstate* s, char endch) {
  char* string = malloc(256);
  size_t string_size = 256;
  
  size_t i = 0;
  while (1) {
    if (isspace(s->ch) || s->ch == endch) {
      break;
    }
    
    switch (s->ch) {
      case ';':
        goto break_loop;
      break;
      case EOF:
        error(s, "Unexpected EOF");
      break;
    }
    
    string[i] = s->ch;
    i++;
    consume(s);
    
    if (i >= string_size) {
      string_size *= 2;
      string = realloc(string, string_size);
    }
  }
  break_loop:
  
  string[i] = '\0';
  
  return string;
}

static char* get_string(lexstate* s) {
  char* string = malloc(256);
  size_t string_size = 256;
  
  size_t i = 0;
  while (1) {
    switch (s->ch) {
      case '"':
        goto break_loop;
      break;
      case '\\':
        consume(s);
        
        switch (s->ch) {
          case '"':
            string[i] = '"';
          break;
          case '\\':
            string[i] = '\\';
          break;
          default:
            error(s, "Unknown escape character");
          break;
        }
        
      break;
      case EOF:
        error(s, "Unexpected EOF");
      break;
      default:
        string[i] = s->ch;
      break;
    }
    
    i++;
    consume(s);
    
    if (i >= string_size) {
      string_size *= 2;
      string = realloc(string, string_size);
    }
  }
  break_loop:
  
  consume(s);
  string[i] = '\0';
  
  return string;
}

static void recurse(lexstate* s, char endch, lextype endtype) {
  while (1) {
    skip_space(s);
    
    if (s->ch == endch) {
      consume(s);
      break;
    }
    
    switch (s->ch) {
      case ';':
        skip_until_newline(s);
      break;
      case '(':
        put_token(s, BEGIN_BLOCK, NULL);
        consume(s);
        
        recurse(s, ')', END_BLOCK);
      break;
      case '\'':
        consume(s);
        
        if (s->ch == '(') {
          put_token(s, BEGIN_LIST, NULL);
          consume(s);
          recurse(s, ')', END_LIST);
        } else {
          char* literal = get_until_disallowed(s, endch);
          put_token(s, LITERAL, literal);
        }
      break;
      case '[':
        put_token(s, BEGIN_VECTOR, NULL);
        consume(s);
        
        recurse(s, ']', END_VECTOR);
      break;
      case ':':
        consume(s);
        
        char* keyword = get_until_disallowed(s, endch);
        put_token(s, KEYWORD, keyword);
      break;
      case '"':
        consume(s);
        
        char* string = get_string(s);
        put_token(s, STRING, string);
      break;
      default:
        char* symbol = get_until_disallowed(s, endch);
        put_token(s, SYMBOL, symbol);
      break;
    }
  }
  
  put_token(s, endtype, NULL);
}

void lex(lexstate* s) {
  while (1) {
    skip_space(s);
    
    switch (s->ch) {
      case ';':
        skip_until_newline(s);
      break;
      case '(':
        put_token(s, BEGIN_BLOCK, NULL);
        consume(s);
        
        recurse(s, ')', END_BLOCK);
      break;
      case EOF:
        goto break_loop;
      break;
      default:
        error(s, "Expected ';', '(' or EOF");
      break;
    }
  }
  break_loop:
  
  put_token(s, END_TOKENS, NULL);
}