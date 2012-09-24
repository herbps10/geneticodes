#ifndef VERTEX_QUEUE_HEADER
#define VERTEX_QUEUE_HEADER

#include "base_node.h"
#include "node_pool.h"

struct queue_node_struct
{
  struct queue_node_struct *next;
  struct queue_node_struct *prev;

  base_node *node;
};

typedef struct queue_node_struct queue_node;

struct base_queue_struct
{
	struct queue_node_struct *head;
	struct queue_node_struct *tail;

	int length;
};

typedef struct base_queue_struct base_queue;

base_queue *base_queue_create();
void base_queue_init(base_queue *);

void base_queue_enqueue(base_queue *, base_node *);
base_node *base_queue_top(base_queue *queue);
base_node *base_queue_dequeue(base_queue *queue);


void base_queue_inspect(base_queue *);

#endif
