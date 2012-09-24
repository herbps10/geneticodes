#include "base_node.h"

base_node *base_node_create()
{
	base_node *node = malloc(sizeof(base_node));

	if(node == NULL)
	{
		printf("Error instantiating node\n");
	}

	base_node_init(node);

	return node;
}

void base_node_init(base_node *node)
{
	node->next = NULL;
	node->prev = NULL;
	node->base = NULL;
}
