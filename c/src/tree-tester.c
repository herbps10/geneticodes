#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define EQN_GENE_HEAD_LENGTH 4
#define EQN_GENE_TAIL_LENGTH 5 
#define EQN_GENE_LENGTH EQN_GENE_HEAD_LENGTH + EQN_GENE_TAIL_LENGTH

#include "base.c"
#include "queue.c"
#include "base_tree.c"
#include "chromosome.c"
#include "base_node.c"
#include "stack.c"
#include "node_pool.c"

int main(int argc, char *argv[]) {
  chromosome *chromosome1 = chromosome_create();

  chromosome1->gene1[0].value = '-';
  chromosome1->gene1[0].terminal = false; 

  chromosome1->gene1[1].value = '*';
  chromosome1->gene1[1].terminal = false; 

  chromosome1->gene1[2].value = 'A';
  chromosome1->gene1[2].terminal = true; 

  chromosome1->gene1[3].value = 'B';
  chromosome1->gene1[3].terminal = true; 

  chromosome1->gene1[4].value = 'B';
  chromosome1->gene1[4].terminal = true; 

  chromosome_display(chromosome1);

  base_node *root = base_node_create();
  root->base = &chromosome1->gene1[0];

  base_node *left = base_node_create();
  left->base = &chromosome1->gene1[1];
  root->prev = left;

  base_node *right = base_node_create();
  right->base = &chromosome1->gene1[2];
  root->next = right;


  base_node *left2 = base_node_create();
  left2->base = &chromosome1->gene1[3];

  base_node *right2 = base_node_create();
  right2->base = &chromosome1->gene1[4];

  left->prev = left2;
  left->next = right2;

  base_tree_display(root);

  printf("%f", base_tree_eval(root));

//  chromosome_destroy(chromosome1);

  return 0;
}
