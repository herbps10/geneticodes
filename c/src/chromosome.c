#include "chromosome.h"

chromosome *chromosome_create()
{

  chromosome *chromosome = malloc(sizeof(chromosome) + sizeof(base *) * (EQN_GENE_LENGTH) * 3);

  chromosome->beta  = 0.6;
  chromosome->gamma = 0.3;
  chromosome->mu    = 0.02;

  chromosome->gene1 = (base **) malloc(sizeof(base *) * EQN_GENE_LENGTH);
  chromosome->gene2 = (base **) malloc(sizeof(base *) * EQN_GENE_LENGTH);
  chromosome->gene3 = (base **) malloc(sizeof(base *) * EQN_GENE_LENGTH);

  init_gene(chromosome->gene1);
  init_gene(chromosome->gene2);
  init_gene(chromosome->gene3);
  
  return chromosome;
}

void init_gene(base **gene)
{
  for(int i = 0; i < EQN_GENE_LENGTH; i++)
  {
    gene[i] = base_create();
  }
}

void set_gene(base **gene, char *genestr)
{
  for(int i = 0; i < EQN_GENE_LENGTH; i++)
  {
    gene[i]->value = genestr[i];

    gene[i]->terminal = true;
    if(genestr[i] == '+' || genestr[i] == '-' || genestr[i] == '*')
    {
      gene[i]->terminal = false;
    }

  }
}

void gene_display(base **gene)
{
  // Display the first gene
  for(int i = 0; i < EQN_GENE_LENGTH; i++)
  {
    printf("%c ", gene[i]->value);
  }

  printf("\n");
}

void chromosome_display(chromosome *c)
{
  printf("Gene 1: ");
  gene_display(c->gene1);

  printf("Gene 2: ");
  gene_display(c->gene2);

  printf("Gene 3: ");
  gene_display(c->gene3);
}

void chromosome_destroy(chromosome *c)
{
  free(c);
  free(c->gene1);
  free(c->gene2);
  free(c->gene3);
}

base_node *gene_to_tree(base **gene)
{

  base_queue *operators = base_queue_create();
  base_queue *next_row_operators = base_queue_create();

  base_node *root = base_node_create();
  root->base = gene[0];
  
  base_queue_enqueue(operators, root);

  int base_index = 1;
  while(operators->length > 0)
  {
    base_node *operator = base_queue_top(operators);

    if(operator->prev != NULL && operator->next != NULL)
    {
      base_queue_dequeue(operators);
      operator = base_queue_top(operators);

      if(operator == NULL)
      {
        break;
      }
    }


    if(operator->prev == NULL)
    {
      operator->prev = base_node_create();
      operator->prev->base = gene[base_index];

      if(operator->prev->base->terminal == false)
      { 
        base_queue_enqueue(operators, operator->prev);
      }
    }
    else if(operator->next == NULL)
    {
      operator->next = base_node_create();
      operator->next->base = gene[base_index];

      if(operator->next->base->terminal == false)
      { 
        base_queue_enqueue(operators, operator->next);
      }
    }

    base_index += 1;
  }

  base_queue_simple_destroy(operators);

  return root;
}


void chromosome_eval(chromosome *c, double *S_results, double *I_results, double *R_results)
{
  // Runge-Kutta Approximation of the System

  // Start with just gene1, which we will take as the I equation
  
  // First, convert the gene into an expression tree
  base_node *tree_s = gene_to_tree(c->gene1);
  base_node *tree_i = gene_to_tree(c->gene2);
  base_node *tree_r = gene_to_tree(c->gene3);

  // Starting parameters
  double S = 0.999;
  double I = 0.001;
  double R = 0;

  double S_next, I_next, R_next;
  S_next = I_next = R_next;

  for(int timestep = 0; timestep < ODE_TIMESTEPS; timestep++) 
  {

    S_results[timestep] = S;
    I_results[timestep] = I;
    R_results[timestep] = R;

    S_next = ode_step(c, tree_s, S, I, R);
    I_next = ode_step(c, tree_i, S, I, R);
    R_next = ode_step(c, tree_r, S, I, R);

    S += S_next;
    I += I_next;
    R += R_next;

    if(S > 1) S = 1;
    if(I > 1) I = 1;
    if(R > 1) R = 1;

    if(S < 0) S = 0;
    if(I < 0) I = 0;
    if(R < 0) R = 0;

  }

  base_tree_destroy(tree_s);
  base_tree_destroy(tree_i);
  base_tree_destroy(tree_r);

}

void gene_randomize(base **gene)
{
  char *bases = "SIR01!gb+-*";

  for(int i = 0; i < EQN_GENE_LENGTH; i++)
  {
    if(i < EQN_GENE_HEAD_LENGTH)
    {
      int choose_terminal = rand_lim(HEAD_TERMINAL_PROBABILITY);
      
      int index;
      if(choose_terminal == 0)
      {
        // Choose a terminal
        index = rand_lim(NUM_TERMINALS - 1);
      }
      else
      {
        // Choose an operator
        index = NUM_TERMINALS + rand_lim(NUM_OPERATORS - 1);
      }

      gene[i]->value = bases[index];
      gene[i]->terminal = true;

      if(bases[index] == '+' || bases[index] == '-' || bases[index] == '*')
      {
        gene[i]->terminal = false;
      }
    }
    else
    {
      int index = rand_lim(NUM_TERMINALS - 1);

      gene[i]->value = bases[index];
      gene[i]->terminal = true;
    }
  }
}

void chromosome_randomize(chromosome *c)
{
  gene_randomize(c->gene1);
  gene_randomize(c->gene2);
  gene_randomize(c->gene3);
}

double chromosome_fitness(chromosome *c, double *target_S, double *target_I, double *target_R)
{
  double S_results[ODE_TIMESTEPS];
  double I_results[ODE_TIMESTEPS];
  double R_results[ODE_TIMESTEPS];

  chromosome_eval(c, S_results, I_results, R_results);

  double sum_of_squares = 0;

  for(int i = 0; i < ODE_TIMESTEPS; i++)
  {
    sum_of_squares += pow(target_S[i] - S_results[i], 2);
    sum_of_squares += pow(target_I[i] - I_results[i], 2);
    sum_of_squares += pow(target_R[i] - R_results[i], 2);
  }

  c->fitness = sum_of_squares;

  return sum_of_squares;
}

inline double ode_step(chromosome *c, base_node *parse_tree, double S, double I, double R)
{
  double y_next;

  y_next = base_tree_eval(c, parse_tree, S, I, R);

  return y_next;
}

// Randomly mutates one of the bases in a gene
void gene_point_mutate(base **gene)
{
  int index = rand_lim(EQN_GENE_LENGTH - 1);

  char *bases = "SIR1!0gb+-*";
  
  if(index < EQN_GENE_HEAD_LENGTH)
  {
    int new_index = rand_lim(NUM_BASES - 1);

    // Head bases can be mutated to any other base. There is no restriction.

    gene[index]->value = bases[new_index];

    gene[index]->terminal = true;
    if(bases[new_index] == '+' || bases[new_index] == '-' || bases[new_index] == '*')
    {
      gene[index]->terminal = false;
    }

  }
  else
  {
    int new_index = rand_lim(NUM_TERMINALS - 1);
    // Tail bases can only be mutated from a terminator to another terminator.
    gene[index]->value = bases[new_index];
  }
}

void gene_mutate(base **gene)
{
  double r = rand_one();

  if(r < QUADRUPLE_MUTATION_PROBABILITY)
  {
    gene_point_mutate(gene);
    gene_point_mutate(gene);
    gene_point_mutate(gene);
    gene_point_mutate(gene);
  }
  if(r < TRIPLE_MUTATION_PROBABILITY)
  {
    gene_point_mutate(gene);
    gene_point_mutate(gene);
    gene_point_mutate(gene);
  }
  else if(r < DOUBLE_MUTATION_PROBABILITY)
  {
    gene_point_mutate(gene);
    gene_point_mutate(gene);
  }
  else if(r < MUTATION_PROBABILITY)
  {
    gene_point_mutate(gene);
  }
}

void genes_recombine(base **gene1, base **gene2, base **target)
{
  int crossover_index = rand_lim(EQN_GENE_LENGTH - 1);

  for(int i = 0; i < EQN_GENE_LENGTH; i++)
  {
    if(i <= crossover_index)
    {
      target[i]->value    = gene1[i]->value;
      target[i]->terminal = gene1[i]->terminal;
    }
    else
    {
      target[i]->value    = gene2[i]->value;
      target[i]->terminal = gene2[i]->terminal;
    }
  }
}

void genes_exchange(base **gene1, base **gene2, base **target)
{
  int start_index = rand_lim(EQN_GENE_LENGTH - 1);

  int end_index = start_index + rand_lim(EQN_GENE_LENGTH - start_index - 1);
  
  for(int i = 0; i < EQN_GENE_LENGTH; i++)
  {
    if(i < start_index || i > end_index)
    {
      target[i]->value    = gene1[i]->value;
      target[i]->terminal = gene1[i]->terminal;
    }
    else
    {
      target[i]->value    = gene2[i]->value;
      target[i]->terminal = gene2[i]->terminal;
    }
  }
}

void genes_copy(base **gene, base **target)
{
  for(int i = 0; i < EQN_GENE_LENGTH; i++)
  {
    target[i]->value    = gene[i]->value;
    target[i]->terminal = gene[i]->terminal;
  }
}

void chromosomes_copy(chromosome *c, chromosome *target)
{
  genes_copy(c->gene1, target->gene1);
  genes_copy(c->gene2, target->gene2);
  genes_copy(c->gene3, target->gene3);
}

void chromosomes_recombine(chromosome *c1, chromosome *c2, chromosome *target)
{
  if(rand_one() < 0.3333)
  {
    genes_recombine(c1->gene1, c2->gene1, target->gene1);
  }
  else
  {
    genes_copy(c1->gene1, target->gene1);
  }

  if(rand_one() < 0.33333)
  {
    genes_recombine(c1->gene2, c2->gene2, target->gene2);
  }
  else
  {
    genes_copy(c1->gene2, target->gene2);
  }

  if(rand_one() < 0.3333)
  {
    genes_recombine(c1->gene3, c2->gene3, target->gene3);
  }
  else
  {
    genes_copy(c1->gene3, target->gene3);
  }
}

void chromosomes_exchange(chromosome *c1, chromosome *c2, chromosome *target)
{
  if(rand_one() < 0.3333)
  {
    genes_exchange(c1->gene1, c2->gene1, target->gene1);
  }
  else
  {
    genes_copy(c1->gene1, target->gene1);
  }


  if(rand_one() < 0.33333)
  {
    genes_exchange(c1->gene2, c2->gene2, target->gene2);
  }
  else
  {
    genes_copy(c1->gene2, target->gene2);
  }

  if(rand_one() < 0.3333)
  {
    genes_exchange(c1->gene3, c2->gene3, target->gene3);
  }
  else
  {
    genes_copy(c1->gene3, target->gene3);
  }
}

// Handles the mutation for all the genes in the chromosome
void chromosome_mutate(chromosome *c)
{
  gene_mutate(c->gene1);
  gene_mutate(c->gene2);
  gene_mutate(c->gene3);
}

int chromosome_compare(const void *a, const void *b)
{
  chromosome **c1 = (chromosome **) a;
  chromosome **c2 = (chromosome **) b;

  return ((*c1)->fitness > (*c2)->fitness ? 1 : -1);
}
