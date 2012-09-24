#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define EQN_GENE_HEAD_LENGTH 6
#define EQN_GENE_TAIL_LENGTH 7
#define EQN_GENE_LENGTH (EQN_GENE_HEAD_LENGTH + EQN_GENE_TAIL_LENGTH)

#define NUM_BASES 11
#define NUM_TERMINALS 8
#define NUM_OPERATORS 3

#define HEAD_TERMINAL_PROBABILITY 3 // The chance of a terminal being generated in the head is 1/HEAD_TERMINAL_PROBABILITY

#define ODE_TIMESTEPS 100

#define MUTATION_PROBABILITY 0.2
#define DOUBLE_MUTATION_PROBABILITY 0.05
#define TRIPLE_MUTATION_PROBABILITY 0.05
#define QUADRUPLE_MUTATION_PROBABILITY 0.00

#define POPULATION_SIZE 100000
#define SURVIVOR_SIZE   50000

#define GENERATIONS 1000

#define NUM_THREADS 4

// from http://stackoverflow.com/questions/2999075/generate-a-random-number-within-range/2999130#2999130
int rand_lim(int limit) {
/* return a random number between 0 and limit inclusive.
 */

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

double rand_one()
{
  return (double)rand() / (double)RAND_MAX;
}


#include "base.c"
#include "queue.c"
#include "base_tree.c"
#include "chromosome.c"
#include "base_node.c"
#include "stack.c"
#include "node_pool.c"

struct thread_data
{
  chromosome **population;
  int threadid;
  double *target_S;
  double *target_I;
  double *target_R;
};

struct thread_data thread_data_array[NUM_THREADS];
int threads_running[NUM_THREADS];
pthread_mutex_t running_mutex = PTHREAD_MUTEX_INITIALIZER;

void *fitness_thread(void *thread_data)
{
  struct thread_data *data = (struct thread_data *) thread_data;

  int id = data->threadid;
  chromosome **population = data->population;

  double *target_S = data->target_S;
  double *target_I = data->target_I;
  double *target_R = data->target_R;

  int range = POPULATION_SIZE / NUM_THREADS;

  int start = id * range;
  int end = start + range;

  for(int i = start; i < end; i++)
  {
    chromosome_fitness(population[i], target_S, target_I, target_R);
  }


  pthread_mutex_lock(&running_mutex);
  threads_running[id] = 0;
  pthread_mutex_unlock(&running_mutex);
  
  pthread_exit(NULL);
}

void update_fitnesses(chromosome **population, double *target_S, double *target_I, double *target_R)
{
  pthread_t threads[NUM_THREADS];

  for(int thread = 0; thread < NUM_THREADS; thread++)
  {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    thread_data_array[thread].threadid = thread;
    thread_data_array[thread].target_S = target_S;
    thread_data_array[thread].target_I = target_I;
    thread_data_array[thread].target_R = target_R;
    thread_data_array[thread].population = population;


    pthread_mutex_lock(&running_mutex);
    threads_running[thread] = 1;
    pthread_mutex_unlock(&running_mutex);

    threads[thread] = pthread_create(&threads[thread], &attr, fitness_thread, (void *) &thread_data_array[thread]);


    pthread_attr_destroy(&attr);
  }

  for(int thread = 0; thread < NUM_THREADS; thread++)
  {
    while(threads_running[thread] == 1);
    int state = pthread_join(threads[thread], NULL);
  }
}


int main(int argc, char *argv[]) {
  srand(time(NULL));

  chromosome *target_chromosome = chromosome_create();

  char *target_gene1 = "***1bSIIIIIII";
  char *target_gene2 = "-**b*gISIIIII";
  char *target_gene3 = "*gIIIIIIIIIII";

  set_gene(target_chromosome->gene1, target_gene1);
  set_gene(target_chromosome->gene2, target_gene2);
  set_gene(target_chromosome->gene3, target_gene3);

  double target_S[ODE_TIMESTEPS];
  double target_I[ODE_TIMESTEPS];
  double target_R[ODE_TIMESTEPS];

  chromosome_eval(target_chromosome, target_S, target_I, target_R);

  /*
  base_tree_display(gene_to_tree(target_chromosome->gene1));
  printf("\n");

  base_tree_display(gene_to_tree(target_chromosome->gene2));
  printf("\n");

  base_tree_display(gene_to_tree(target_chromosome->gene3));
  printf("\n");
  */


  ///////////////
  // ALGORITHM //
  ///////////////

  chromosome *population[POPULATION_SIZE];

  // Randomize initial population
  for(int chr_index = 0; chr_index < POPULATION_SIZE; chr_index++)
  {
      population[chr_index] = chromosome_create();
      chromosome_randomize(population[chr_index]);

      //set_gene(population[chr_index]->gene1, target_gene1);
      //set_gene(population[chr_index]->gene3, target_gene3);
  }

  ///////////////////////
  // GENETIC ITERATION //
  ///////////////////////

  for(int generation = 0; generation < GENERATIONS; generation++)
  {
    // Calculate fitnesses
    update_fitnesses(population, target_S, target_I, target_R);

    // Sort by fitness
    qsort(population, POPULATION_SIZE, sizeof(population) / POPULATION_SIZE, chromosome_compare);

    // Get the average fitness
    double avg = 0;
    
    for(int i = 0; i < POPULATION_SIZE; i++)
    {
      avg += population[i]->fitness / POPULATION_SIZE;
    }

    /*
    printf("Average fitness: %f\n", avg);
    printf("Average worst fitness: %f\n", population[POPULATION_SIZE - 1]->fitness);
    printf("Average median fitness: %f\n", population[POPULATION_SIZE / 2]->fitness);
    */
    printf("Generation %i best fitness: %f, average: %f\n", generation, population[0]->fitness, avg);

    if(population[0]->fitness < 0.0005) {
      printf("Found zero\n");
      break;
    }

    // Let the best ones go to the next generation, although they mutate.
    // The worst ones are regenerated from the best ones.

    for(int chr_index = POPULATION_SIZE - 1; chr_index >= 0; chr_index--)
    {
      if(chr_index < SURVIVOR_SIZE)
      {
        // These are the ones that survive to the next generation
        chromosome_mutate(population[chr_index]);
      }
      else
      {
        // These ones are killed off and regenerated
        
        int parent1 = rand_lim(SURVIVOR_SIZE - 1);

        // Get the second parent, making sure it isn't the same as the first one
        int parent2;
        do
        {
          parent2 = rand_lim(SURVIVOR_SIZE - 1);
        }
        while(parent2 == parent1);

        if(rand_one() < 0.5)
        {
          chromosomes_recombine(population[parent1], population[parent2], population[chr_index]);
        }
        else
        {
          chromosomes_exchange(population[parent1], population[parent2], population[chr_index]);
        }

        chromosome_mutate(population[chr_index]);
      }
    }

  base_tree_display(gene_to_tree(population[0]->gene1));
  printf("\n");

  base_tree_display(gene_to_tree(population[0]->gene2));
  printf("\n");

  base_tree_display(gene_to_tree(population[0]->gene3));
  printf("\n");


  }
  chromosome_destroy(target_chromosome);

  return 0;
}


void test_chromosome_copier()
{
  chromosome *c1 = chromosome_create();
  char *c1_gene1 = "PPPPPPPPPPPPPPPPPP";
  char *c1_gene2 = "PPPPPPPPPPPPPPPPPP";
  char *c1_gene3 = "PPPPPPPPPPPPPPPPPP";

  set_gene(c1->gene1, c1_gene1);
  set_gene(c1->gene2, c1_gene2);
  set_gene(c1->gene3, c1_gene3);

  chromosome *c2 = chromosome_create();
  char *c2_gene1 = "AAAAAAAAAAAAAAAAAA";
  char *c2_gene2 = "AAAAAAAAAAAAAAAAAA";
  char *c2_gene3 = "AAAAAAAAAAAAAAAAAA";
  
  set_gene(c2->gene1, c2_gene1);
  set_gene(c2->gene2, c2_gene2);
  set_gene(c2->gene3, c2_gene3);

  gene_display(c1->gene1);
  gene_display(c2->gene1);


  chromosome *c3 = chromosome_create();

  chromosomes_copy(c2, c3);

  gene_display(c3->gene1);
  gene_display(c3->gene2);
  gene_display(c3->gene3);
}
