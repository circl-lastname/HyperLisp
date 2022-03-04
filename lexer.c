#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "shared.h"
#include "lexer.h"

static void error(lexstate* s, const char* string) {
  print_error_and_exit(s->file, s->filename, s->curline, s->curchar, string);
}

static void set_token_cur(lexstate* s) {
  s->token_curline = s->curline;
  s->token_curchar = s->curchar;
}

static void put_token(lexstate* s, lextype type, char* data) {
  lextoken* token = &s->tokens[s->tokens_amount];
  
  token->type = type;
  token->data = data;
  token->curline = s->token_curline;
  token->curchar = s->token_curchar;
  
  s->tokens_amount++;
  
  if (s->tokens_amount >= s->tokens_size) {
    s->tokens_size *= 2;
    s->tokens = try(realloc(s->tokens, s->tokens_size*sizeof(lextoken)));
  }
}

static void consume(lexstate* s) {
  s->ch = fgetc(s->file);
  
  // this is a big if block to run every character, but
  // - it can't run during printing an error
  // - it needs to mark the next character after a newline for reset
  // - it can't update on an EOF, since it can result in unhelpful errors
  if (!s->counter_disabled) {
    if (s->after_newline) {
      if (s->ch != EOF) {
        s->after_newline = 0;
        s->curline++;
        s->curchar = 0;
      }
    } else {
      s->curchar++;
    }
    
    switch (s->ch) {
      case '\n':
        s->after_newline = 1;
      break;
      case EOF:
        s->counter_disabled = 1;
      break;
    }
  }
}

static void skip_space(lexstate* s) {
  while (isspace(s->ch)) {
    consume(s);
  }
}

static void skip_until_newline(lexstate* s) {
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
    
    consume(s);
  }
  break_loop:
  
  consume(s);
}

static char* get_until_disallowed(lexstate* s) {
  char* string = try(malloc(256));
  size_t string_size = 256;
  
  size_t i = 0;
  while (1) {
    if (isspace(s->ch)) {
      break;
    }
    
    switch (s->ch) {
      case ';':
        goto break_loop;
      break;
      case '\'':
        goto break_loop;
      break;
      case '"':
        goto break_loop;
      break;
      case '(':
        goto break_loop;
      break;
      case ')':
        goto break_loop;
      break;
      case '[':
        goto break_loop;
      break;
      case ']':
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
      string = try(realloc(string, string_size));
    }
  }
  break_loop:
  
  string[i] = '\0';
  
  return string;
}

static char* get_string(lexstate* s) {
  char* string = try(malloc(256));
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
          case 'n':
            string[i] = '\n';
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
      string = try(realloc(string, string_size));
    }
  }
  break_loop:
  
  consume(s);
  string[i] = '\0';
  
  return string;
}

static void recurse(lexstate* s, lextype endtype) {
  while (1) {
    skip_space(s);
    
    switch (s->ch) {
      case ';':
        skip_until_newline(s);
      break;
      case ':':
        set_token_cur(s);
        consume(s);
        
        char* keyword = get_until_disallowed(s);
        put_token(s, KEYWORD, keyword);
      break;
      case '\'':
        set_token_cur(s);
        consume(s);
        
        if (s->ch == '(') {
          put_token(s, BEGIN_LIST, NULL);
          consume(s);
          recurse(s, END_LIST);
        } else {
          char* literal = get_until_disallowed(s);
          put_token(s, LITERAL, literal);
        }
      break;
      case '"':
        set_token_cur(s);
        consume(s);
        
        char* string = get_string(s);
        put_token(s, STRING, string);
      break;
      case '(':
        set_token_cur(s);
        put_token(s, BEGIN_BLOCK, NULL);
        consume(s);
        
        recurse(s, END_BLOCK);
      break;
      case ')':
        if (endtype != END_BLOCK && endtype != END_LIST) {
          error(s, "Unexpected ')'");
        }
        
        goto break_loop;
      break;
      case '[':
        set_token_cur(s);
        put_token(s, BEGIN_VECTOR, NULL);
        consume(s);
        
        recurse(s, END_VECTOR);
      break;
      case ']':
        if (endtype != END_VECTOR) {
          error(s, "Unexpected ']'");
        }
        
        goto break_loop;
      break;
      default: { // some compilers (eg clang, older gcc) don't like having variables in default
        set_token_cur(s);
        char* symbol = get_until_disallowed(s);
        put_token(s, SYMBOL, symbol);
      } break;
    }
  }
  break_loop:
  
  set_token_cur(s);
  put_token(s, endtype, NULL);
  consume(s);
}

void lex(lexstate* s) {
  switch (s->ch) {
    case '\n':
      s->after_newline = 1;
    break;
    case EOF:
      s->counter_disabled = 1;
    break;
  }
  
  while (1) {
    skip_space(s);
    
    switch (s->ch) {
      case ';':
        skip_until_newline(s);
      break;
      case '(':
        set_token_cur(s);
        put_token(s, BEGIN_BLOCK, NULL);
        consume(s);
        
        recurse(s, END_BLOCK);
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
  
  set_token_cur(s);
  put_token(s, END_TOKENS, NULL);
}
