#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

static void error(prsstate* s, const char* string) {
  fprintf(stderr, "%s:%i:%i  %s\n", s->filename_in, s->tk->curline+1, s->tk->curchar+1, string);
  exit(1);
}

static void consume(prsstate* s) {
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

static void recurse(prsstate* s) {
  if (s->tk->type != BEGIN_BLOCK) {
    error(s, "Expected BEGIN_BLOCK");
  }
  consume(s);
  
  if (s->tk->type != SYMBOL) {
    error(s, "A tag must be a symbol");
  }
  
  fprintf(s->file, "<%s ", s->tk->data);
  char* element = s->tk->data;
  consume(s);
  
  while (s->tk->type == KEYWORD) {
    fprintf(s->file, "%s", s->tk->data);
    consume(s);
    
    if (!(s->tk->type == STRING || s->tk->type == LITERAL)) {
      error(s, "An attribute must be a string or literal");
    }
    
    if (s->tk->data[0] != '\0') {
      fprintf(s->file, "=\"%s\" ", s->tk->data);
    } else {
      fprintf(s->file, " ");
    }
    
    consume(s);
  }
  
  fprintf(s->file, ">");
  
  if (needs_closing(element)) {
    while (s->tk->type != END_BLOCK) {
      switch (s->tk->type) {
        case BEGIN_BLOCK:
          recurse(s);
        break;
        case STRING:
        case LITERAL:
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
    if (s->tk->type != END_BLOCK) {
      error(s, "Content is not allowed in void elements");
    }
  }
}

void parse(prsstate* s) {  
  fputs("<!doctype html>", s->file);
  
  recurse(s);
}
