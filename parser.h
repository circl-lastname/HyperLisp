typedef struct {
  FILE* file;
  
  const char* filename_in;
  
  lextoken* tk;
  long current_token;
  
  int put_doctype;
  
  long tokens_amount;
  lextoken* tokens;
} prsstate;

void parse(prsstate* s);
