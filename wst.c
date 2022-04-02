#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "shared.h"
#include "wst.h"

wst_tree* wst_create() {
  wst_tree* tree = try(malloc(256*sizeof(wst_tree)));
  
  for (int i = 0; i < 256; i++) {
    tree[i].value = NULL;
    tree[i].member = NULL;
  }
  
  return tree;
}

void wst_free(wst_tree* tree) {
  for (int i = 0; i < 256; i++) {
    if (tree[i].member) {
      wst_free(tree[i].member);
    }
  }
  
  free(tree);
}

wst_tree* wst_find(wst_tree* tree, char* name, int create) {
  int i = 0;
  wst_tree* c_tree = tree;
  
  for (; i < strlen(name)-1; i++) {
    if (!c_tree[name[i]].member) {
      if (create) {
        c_tree[name[i]].member = wst_create();
      } else {
        return NULL;
      }
    }
    
    c_tree = c_tree[name[i]].member;
  }
  
  return &c_tree[name[i]];
}
