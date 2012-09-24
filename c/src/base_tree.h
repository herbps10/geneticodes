#ifndef BASE_TREE_HEADER
#define BASE_TREE_HEADER

#include "base.h"
#include "base_node.h"
#include "chromosome.h"

struct base_tree_struct
{
  base_node *root;
};

typedef struct base_tree_struct base_tree;

base_tree *base_tree_create(base *);

#endif
