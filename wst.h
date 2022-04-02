struct wst_tree {
  void* value;
  struct wst_tree* member;
};
typedef struct wst_tree wst_tree;

wst_tree* wst_create();
void wst_free(wst_tree* tree);
wst_tree* wst_find(wst_tree* tree, char* name, int create);

static inline void wst_set(wst_tree* tree, char* name, void* ptr) {
  wst_find(tree, name, 1)->value = ptr;
}

static inline void* wst_get(wst_tree* tree, char* name) {
  wst_tree* found = wst_find(tree, name, 0);
  if (!found) { 
    return NULL;
  }
  
  return found->value;
}
