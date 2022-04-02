#include "wst.h"

typedef struct {
  FILE* file;
  
  FILE* file_in;
  const char* filename_in;
  
  lextoken* tk;
  long current_token;
  
  wst_tree* elements;
  int put_doctype;
  
  long tokens_amount;
  lextoken* tokens;
} prsstate;

void parse(prsstate* s);
