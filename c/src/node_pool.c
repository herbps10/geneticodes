#include "node_pool.h"

base_node_pool *base_node_pool_create()
{
	base_node_pool *pool = malloc(sizeof(struct base_node_pool_struct));

	pool->stack = base_stack_create();

	return pool;
}

void base_node_pool_free(base_node_pool *pool, struct base_node_struct *node)
{
	base_stack_push(pool->stack, node);
}

base_node *base_node_pool_get(base_node_pool *pool)
{
	if(base_stack_empty(pool->stack))
	{
		base_node *node = base_node_create();

		return node;
	}
	else
	{
		return base_stack_pop(pool->stack);
	}
}
