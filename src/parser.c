#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"
#include "lexer.h"
#include "parser.h"

static void error(prs_state* s, const char* string) {
  print_error_and_exit(s->file_in, s->filename_in, s->tk->cur_line, s->tk->cur_char, string);
}

static void consume(prs_state* s) {
  s->current_token++;
  s->tk = &s->tokens[s->current_token];
}

static int needs_closing(char* element) {
  if (!strcmp(element, "area") ||
      !strcmp(element, "base") ||
      !strcmp(element, "br") ||
      !strcmp(element, "col") ||
      !strcmp(element, "embed") ||
      !strcmp(element, "hr") ||
      !strcmp(element, "img") ||
      !strcmp(element, "input") ||
      !strcmp(element, "link") ||
      !strcmp(element, "meta") ||
      !strcmp(element, "param") ||
      !strcmp(element, "source") ||
      !strcmp(element, "track") ||
      !strcmp(element, "wbr")) {
    return 0;
  } else {
    return 1;
  }
}

static void recurse(prs_state* s) {
  if (s->tk->type != TOKEN_BLOCK_BEGIN) {
    error(s, "Expected TOKEN_BLOCK_BEGIN");
  }
  consume(s);
  
  if (s->tk->type != TOKEN_SYMBOL) {
    error(s, "A tag must be a symbol");
  }
  
  char* element = s->tk->data;
  for (size_t i = 0; i < strlen(element); i++) {
    element[i] = tolower(element[i]);
  }
  
  if (!strcmp(element, "!nodoctype")) {
    if (!s->put_doctype) {
      s->put_doctype = true;
      
      consume(s);
      if (s->tk->type != TOKEN_BLOCK_END) {
        error(s, "Content and attributes are not allowed in a !nodoctype");
      }
      return;
    } else {
      error(s, "A !nodoctype must be in the beggining of the file and cannot be called twice");
    }
  }
  
  if (!s->put_doctype) {
    fputs("<!doctype html>", s->file);
    s->put_doctype = true;
  }
  
  fprintf(s->file, "<%s", element);
  consume(s);
  
  while (s->tk->type == TOKEN_KEYWORD) {
    fprintf(s->file, " %s", s->tk->data);
    consume(s);
    
    if (!(s->tk->type == TOKEN_STRING || s->tk->type == TOKEN_LITERAL)) {
      error(s, "An attribute must be a string or literal");
    }
    
    if (s->tk->data[0] != '\0') {
      fprintf(s->file, "=\"%s\"", s->tk->data);
    }
    
    consume(s);
  }
  
  fputc('>', s->file);
  
  if (needs_closing(element)) {
    while (s->tk->type != TOKEN_BLOCK_END) {
      switch (s->tk->type) {
        case TOKEN_BLOCK_BEGIN:
          recurse(s);
        break;
        case TOKEN_STRING:
        case TOKEN_LITERAL:
          fputs(s->tk->data, s->file);
        break;
        default:
          error(s, "Content must be other elements, strings or literals");
        break;
      }
      
      consume(s);
    }
    
    fprintf(s->file, "</%s>", element);
  } else {
    if (s->tk->type != TOKEN_BLOCK_END) {
      error(s, "Content is not allowed in void elements");
    }
  }
}

void parse(prs_state* s) {
  while (s->tk->type != TOKEN_EOF) {
    recurse(s);
    consume(s);
  }
}
