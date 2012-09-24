#include "stack.h"

base_stack *base_stack_create()
{
	base_stack *stack = malloc(sizeof(stack));

	if(stack == NULL)
	{
		printf("Cannot instantiate stack\n");
	}

	base_stack_init(stack);
	
	return stack;
}

void base_stack_init(base_stack *stack)
{
	stack->head = NULL;
}

void base_stack_push(struct base_stack_struct *stack, struct base_node_struct *node)
{
	if(stack->head != NULL)
	{
		stack->head->prev = node;
	}

	node->prev = NULL;
	node->next = stack->head;

	stack->head = node;
}

base_node *base_stack_pop(struct base_stack_struct *stack)
{
	if(stack->head == NULL)
	{
		return NULL;
	}
	else
	{
		base_node *temp = stack->head;
		stack->head = stack->head->next;

		if(stack->head != NULL)
		{
			stack->head->prev = NULL;
		}

		return temp;
	}
}

base_node *base_stack_top(base_stack *stack)
{
	return stack->head;
}

bool base_stack_empty(base_stack *stack)
{
	if(stack->head == NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void base_stack_inspect(base_stack *stack)
{
	base_node *iterator = stack->head;
	while(iterator != NULL)
	{
		//printf("id: %i, address: %p, prev: %p, next: %p\n", iterator->base->id, iterator, iterator->prev, iterator->next);
		iterator = iterator->next;
	}
}

void base_stack_destroy(base_node_pool *pool, base_stack *stack)
{
	base_node *node;
	while(base_stack_top(stack) != NULL)
	{
		node = base_stack_pop(stack);
		node->base = NULL;
		base_node_pool_free(pool, node);
	}

	stack->head = NULL;
}
