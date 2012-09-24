#ifndef BASE_STACK_HEADER
#define BASE_STACK_HEADER

#include "node_pool.h"

struct base_stack_struct
{
	struct base_node_struct *head;
};

typedef struct base_stack_struct base_stack;

base_stack *base_stack_create();
void base_stack_init(base_stack *);

void base_stack_push(struct base_stack_struct *, struct base_node_struct *);
base_node *base_stack_pop(struct base_stack_struct *);
base_node *base_stack_top(struct base_stack_struct *);
bool base_stack_empty(struct base_stack_struct *);
void base_stack_destroy(base_node_pool *, base_stack *);

#endif
