#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"
#include "lexer.h"
#include "parser.h"

typedef enum {
  NORMAL,
  DOESNT_NEED_CLOSING,
  SPECIAL_NODOCTYPE,
} elementtype;

elementtype element_types[] = {
  NORMAL,
  DOESNT_NEED_CLOSING,
  SPECIAL_NODOCTYPE,
};

static void error(prsstate* s, const char* string) {
  print_error_and_exit(s->file_in, s->filename_in, s->tk->curline, s->tk->curchar, string);
}

static void consume(prsstate* s) {
  s->current_token++;
  s->tk = &s->tokens[s->current_token];
}

static void recurse(prsstate* s) {
  if (s->tk->type != BEGIN_BLOCK) {
    error(s, "Expected BEGIN_BLOCK");
  }
  consume(s);
  
  if (s->tk->type != SYMBOL) {
    error(s, "A tag must be a symbol");
  }
  
  char* element = s->tk->data;
  elementtype* element_type = (elementtype*)wst_get(s->elements, s->tk->data);
  
  if (!element_type) {
    element_type = &element_types[NORMAL];
  }
  
  if (*element_type == SPECIAL_NODOCTYPE) {
    if (!s->put_doctype) {
      s->put_doctype = 1;
      
      consume(s);
      if (s->tk->type != END_BLOCK) {
        error(s, "Content and attributes are not allowed in a !nodoctype");
      }
      return;
    } else {
      error(s, "A !nodoctype must be in the beggining of the file and cannot be called twice");
    }
  }
  
  if (!s->put_doctype) {
    fputs("<!doctype html>", s->file);
    s->put_doctype = 1;
  }
  
  fprintf(s->file, "<%s", s->tk->data);
  consume(s);
  
  while (s->tk->type == KEYWORD) {
    fprintf(s->file, " %s", s->tk->data);
    consume(s);
    
    if (!(s->tk->type == STRING || s->tk->type == LITERAL)) {
      error(s, "An attribute must be a string or literal");
    }
    
    if (s->tk->data[0] != '\0') {
      fprintf(s->file, "=\"%s\"", s->tk->data);
    }
    
    consume(s);
  }
  
  fputc('>', s->file);
  
  if (*element_type != DOESNT_NEED_CLOSING) {
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
  s->elements = wst_create();
  wst_set(s->elements, "area",       &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "base",       &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "br",         &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "col",        &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "embed",      &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "hr",         &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "img",        &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "input",      &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "link",       &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "meta",       &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "param",      &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "source",     &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "track",      &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "wbr",        &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "wbr",        &element_types[DOESNT_NEED_CLOSING]);
  wst_set(s->elements, "!nodoctype", &element_types[SPECIAL_NODOCTYPE]);
  
  while (s->tk->type != END_TOKENS) {
    recurse(s);
    consume(s);
  }
  
  wst_free(s->elements);
}
