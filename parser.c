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
    error(s, "Expected SYMBOL");
  }
  
  fprintf(s->file, "<%s ", s->tk->data);
  char* element = s->tk->data;
  consume(s);
  
  while (s->tk->type == KEYWORD) {
    fprintf(s->file, "%s=", s->tk->data);
    consume(s);
    
    if (!(s->tk->type == STRING || s->tk->type == LITERAL)) {
      error(s, "Expected SYMBOL or LITERAL");
    }
  
    fprintf(s->file, "\"%s\" ", s->tk->data);
    consume(s);
  }
  
  fprintf(s->file, ">");
  
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
        error(s, "Expected BEGIN_BLOCK, STRING or LITERAL");
      break;
    }
    
    consume(s);
  }
  
  if (needs_closing(element)) {
    fprintf(s->file, "</%s>", element);
  }
}

void parse(prsstate* s) {  
  fputs("<!doctype html>", s->file);
  
  recurse(s);
}
