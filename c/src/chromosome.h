#ifndef CHROMOSOME_HEADER
#define CHROMOSOME_HEADER

#include "base.h"

struct chromosome_struct
{

  double beta;   
  double gamma; 
  double mu;  

  double fitness;

  base **gene1;
  base **gene2;
  base **gene3;

  //struct base_tree *gene1_tree;
  //struct base_queue *base_queue;

};

typedef struct chromosome_struct chromosome;

double ode_step(chromosome *, base_node *, double, double, double);
void init_gene(base **gene);

#endif
