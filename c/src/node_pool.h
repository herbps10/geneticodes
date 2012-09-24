#ifndef BASE_NODE_POOL_HEADER
#define BASE_NODE_POOL_HEADER

struct base_node_pool_struct
{
	struct base_stack_struct *stack;
};

typedef struct base_node_pool_struct base_node_pool;

struct base_node_pool_struct *base_node_pool_create();
void base_node_pool_free(struct base_node_pool_struct *, base_node *);
struct base_node_struct *base_node_pool_get(struct base_node_pool_struct *);

#endif
