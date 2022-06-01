typedef struct {
  FILE* file;
  
  FILE* file_in;
  const char* filename_in;
  
  lex_token* tk;
  long current_token;
  
  bool put_doctype;
  
  long tokens_amount;
  lex_token* tokens;
} prs_state;

void parse(prs_state* s);
