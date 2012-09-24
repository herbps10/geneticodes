#include "queue.h"

base_queue *base_queue_create()
{
	base_queue *queue = malloc(sizeof(base_queue));

	if(queue == NULL)
	{
		printf("Error instantiating queue\n");
	}

	base_queue_init(queue);

	return queue;
}

void base_queue_init(base_queue *queue)
{
	queue->head = NULL;
	queue->tail = NULL;

	queue->length = 0;
}

void base_queue_enqueue(base_queue *queue, base_node *node)
{
  queue_node *qnode = malloc(sizeof(queue_node));
  qnode->node = node;

  if(queue->head == NULL)
  {
    queue->tail = qnode;
  }
  else
  {
    queue->head->prev = qnode;
  }

  qnode->prev = NULL;
  qnode->next = queue->head;

  queue->head = qnode;

  queue->length++;
}

base_node *base_queue_top(base_queue *queue)
{
	if(queue->tail == NULL)
	{
		return NULL;
	}

  return queue->tail->node;
}

base_node *base_queue_peek(base_queue *queue)
{
  if(queue->head == NULL)
  {
    return NULL;
  }

  return queue->head->node;
}

base_node *base_queue_dequeue(base_queue *queue)
{
	if(queue->tail == NULL)
	{
		return NULL;
	}

  queue_node *node = queue->tail;

	if(queue->tail->prev == NULL)
	{
		queue->head = NULL;
		queue->tail = NULL;
	}
	else
	{
		queue->tail->prev->next = NULL;
		queue->tail = queue->tail->prev;
	}

	queue->length--;

  base_node *temp = node->node;

  free(node);

	return temp;
}

void base_queue_inspect(base_queue *queue)
{
	queue_node *iterator = queue->head;

	while(iterator != NULL)
	{
		printf("value: %c, address: %p, prev: %p, next: %p\n", iterator->node->base->value, iterator, iterator->prev, iterator->next);
		iterator = iterator->next;
	}
}

void base_queue_destroy(base_node_pool *pool, base_queue *queue)
{
	queue_node *iterator = queue->head;
	queue_node *temp;

	while(iterator != NULL)
	{
		//printf("%p, %p\n", iterator, iterator->next);
		temp = iterator->next;
		base_node_pool_free(pool, iterator->node);
		iterator = temp;
	}

	queue->head = NULL;
	queue->tail = NULL;
}

void base_queue_simple_destroy(base_queue *queue)
{
  queue_node *iterator = queue->head;
  queue_node *temp;

  while(iterator != NULL)
  {
    temp = iterator->next;
    free(iterator);
    iterator = temp;
  }

  free(queue);
}


_Bool base_queue_empty(base_queue *queue)
{
	if(queue->head == NULL)
	{
		return true;
	}

	return false;
}

/*
void base_queue_prepend(base_queue *q1, base_queue *q2)
{
	if(q1->head != NULL && q2->head != NULL)
	{
		q2->tail->next = q1->head;
		q1->head->prev = q2->tail;
		q1->head = q2->head;

		q1->length = q1->length + q2->length;

		q1->head->backtrace = 1;
	}
}
*/
