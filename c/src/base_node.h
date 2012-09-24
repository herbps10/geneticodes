#ifndef VERTEX_NODE_HEADER
#define VERTEX_NODE_HEADER

struct base_node_struct {
	struct base_struct *base;
	struct base_node_struct *next;
	struct base_node_struct *prev;

	int backtrace;
};

typedef struct base_node_struct base_node;

// Creation

base_node *base_node_create();
void base_node_init(base_node *);

#endif
