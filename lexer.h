typedef enum {
  BEGIN_BLOCK,
  END_BLOCK,
  
  BEGIN_LIST,
  END_LIST,
  
  BEGIN_VECTOR,
  END_VECTOR,
  
  SYMBOL,
  KEYWORD,
  LITERAL,
  STRING,
  
  END_TOKENS,
} lextype;

typedef struct {
  lextype type;
  char* data;
  
  int curline;
  int curchar;
} lextoken;

typedef struct {
  FILE* file;
  const char* filename;
  
  char ch;
  int after_newline;
  
  int curline;
  int curchar;
  int in_error;
  
  long tokens_amount;
  size_t tokens_size;
  lextoken* tokens;
} lexstate;

void lex(lexstate* s);
