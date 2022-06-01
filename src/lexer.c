#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "shared.h"
#include "lexer.h"

static void error(lex_state* s, const char* string) {
  print_error_and_exit(s->file, s->filename, s->cur_line, s->cur_char, string);
}

static void set_token_cur(lex_state* s) {
  s->token_cur_line = s->cur_line;
  s->token_cur_char = s->cur_char;
}

static void put_token(lex_state* s, lex_type type, char* data) {
  lex_token* token = &s->tokens[s->tokens_amount];
  
  token->type = type;
  token->data = data;
  token->cur_line = s->token_cur_line;
  token->cur_char = s->token_cur_char;
  
  s->tokens_amount++;
  
  if (s->tokens_amount >= s->tokens_size) {
    s->tokens_size *= 2;
    s->tokens = try(realloc(s->tokens, s->tokens_size*sizeof(lex_token)));
  }
}

static void consume(lex_state* s) {
  s->ch = fgetc(s->file);
  
  // this is a big if block to run every character, but
  // - it can't run during printing an error
  // - it needs to mark the next character after a newline for reset
  // - it can't update on an EOF, since it can result in unhelpful errors
  if (!s->counter_disabled) {
    if (s->after_newline) {
      if (s->ch != EOF) {
        s->after_newline = false;
        s->cur_line++;
        s->cur_char = 0;
      }
    } else {
      s->cur_char++;
    }
    
    switch (s->ch) {
      case '\n':
        s->after_newline = true;
      break;
      case EOF:
        s->counter_disabled = true;
      break;
    }
  }
}

static void skip_space(lex_state* s) {
  while (isspace(s->ch)) {
    consume(s);
  }
}

static void skip_until_newline(lex_state* s) {
  while (true) {
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

static char* get_until_disallowed(lex_state* s) {
  char* string = try(malloc(256));
  size_t string_size = 256;
  
  size_t i = 0;
  while (true) {
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

static char* get_string(lex_state* s) {
  char* string = try(malloc(256));
  size_t string_size = 256;
  
  size_t i = 0;
  while (true) {
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

static void recurse(lex_state* s, lex_type end_type) {
  while (true) {
    skip_space(s);
    
    switch (s->ch) {
      case ';':
        skip_until_newline(s);
      break;
      case ':':
        set_token_cur(s);
        consume(s);
        
        char* keyword = get_until_disallowed(s);
        put_token(s, TOKEN_KEYWORD, keyword);
      break;
      case '\'':
        set_token_cur(s);
        consume(s);
        
        if (s->ch == '(') {
          put_token(s, TOKEN_LIST_BEGIN, NULL);
          consume(s);
          recurse(s, TOKEN_LIST_END);
        } else {
          char* literal = get_until_disallowed(s);
          put_token(s, TOKEN_LITERAL, literal);
        }
      break;
      case '"':
        set_token_cur(s);
        consume(s);
        
        char* string = get_string(s);
        put_token(s, TOKEN_STRING, string);
      break;
      case '(':
        set_token_cur(s);
        put_token(s, TOKEN_BLOCK_BEGIN, NULL);
        consume(s);
        
        recurse(s, TOKEN_BLOCK_END);
      break;
      case ')':
        if (end_type != TOKEN_BLOCK_END && end_type != TOKEN_LIST_END) {
          error(s, "Unexpected ')'");
        }
        
        goto break_loop;
      break;
      case '[':
        set_token_cur(s);
        put_token(s, TOKEN_VECTOR_BEGIN, NULL);
        consume(s);
        
        recurse(s, TOKEN_VECTOR_END);
      break;
      case ']':
        if (end_type != TOKEN_VECTOR_END) {
          error(s, "Unexpected ']'");
        }
        
        goto break_loop;
      break;
      default: { // some compilers (eg clang, older gcc) don't like having variables in default
        set_token_cur(s);
        char* symbol = get_until_disallowed(s);
        put_token(s, TOKEN_SYMBOL, symbol);
      } break;
    }
  }
  break_loop:
  
  set_token_cur(s);
  put_token(s, end_type, NULL);
  consume(s);
}

void lex(lex_state* s) {
  switch (s->ch) {
    case '\n':
      s->after_newline = true;
    break;
    case EOF:
      s->counter_disabled = true;
    break;
  }
  
  while (true) {
    skip_space(s);
    
    switch (s->ch) {
      case ';':
        skip_until_newline(s);
      break;
      case '(':
        set_token_cur(s);
        put_token(s, TOKEN_BLOCK_BEGIN, NULL);
        consume(s);
        
        recurse(s, TOKEN_BLOCK_END);
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
  put_token(s, TOKEN_EOF, NULL);
}
