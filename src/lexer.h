#define ENUMERATE_TOKENS(f) \
  f(TOKEN_BLOCK_BEGIN) \
  f(TOKEN_BLOCK_END) \
  \
  f(TOKEN_LIST_BEGIN) \
  f(TOKEN_LIST_END) \
  \
  f(TOKEN_VECTOR_BEGIN) \
  f(TOKEN_VECTOR_END) \
  \
  f(TOKEN_SYMBOL) \
  f(TOKEN_KEYWORD) \
  f(TOKEN_LITERAL) \
  f(TOKEN_STRING) \
  \
  f(TOKEN_EOF) \

typedef enum {
  #define F(t) t,
  ENUMERATE_TOKENS(F)
  #undef F
} lex_type;

typedef struct {
  lex_type type;
  char* data;
  
  long cur_line;
  long cur_char;
} lex_token;

typedef struct {
  FILE* file;
  const char* filename;
  
  char ch;
  bool after_newline;
  
  long cur_line;
  long cur_char;
  long token_cur_line;
  long token_cur_char;
  bool counter_disabled;
  
  long tokens_amount;
  size_t tokens_size;
  lex_token* tokens;
} lex_state;

void lex(lex_state* s);
