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
  
  long curline;
  long curchar;
} lextoken;

typedef struct {
  FILE* file;
  const char* filename;
  
  char ch;
  int after_newline;
  
  long curline;
  long curchar;
  long token_curline;
  long token_curchar;
  int counter_disabled;
  
  long tokens_amount;
  size_t tokens_size;
  lextoken* tokens;
} lexstate;

void lex(lexstate* s);
