/* Differential Evolution Constants */

#define DIFF_POPULATION_SIZE 100

#define DIFF_CROSSOVER_RATE 0.9
#define DIFF_WEIGHTING_FACTOR 0.8

#define DIFF_GENERATIONS 10

#define BETA 0
#define GAMMA 1
#define MU 2

#define NUM_PARAMETERS 3

void optimize_parameters(chromosome *c, double *target_S, double *target_I, double *target_R) {
  chromosome *test_chromosome = chromosome_create();

  test_chromosome->gene1 = c->gene1;
  test_chromosome->gene2 = c->gene2;
  test_chromosome->gene3 = c->gene3;

  float population[DIFF_POPULATION_SIZE][NUM_PARAMETERS];
  float new_population[DIFF_POPULATION_SIZE][NUM_PARAMETERS];

  float fitnesses[DIFF_POPULATION_SIZE];

  /*
    Generate random population
  */
  for(int row = 0; row < DIFF_POPULATION_SIZE; row++) {
    for(int col = 0; col < NUM_PARAMETERS; col++) {
      population[row][col] = rand_one();
    }
  }

  for(int generation = 0; generation < DIFF_GENERATIONS; generation++) {

    float average_fitness = 0;

    for(int row = 0; row < DIFF_POPULATION_SIZE; row++) {
      
      float new_individual[NUM_PARAMETERS];

      // Get three parents
      int parent1 = -1;
      do {
        rand_lim(DIFF_POPULATION_SIZE - 1);
      } while(parent1 == row);

      int parent2 = -1;
      do {
        parent2 = rand_lim(DIFF_POPULATION_SIZE - 1);
      } while(parent2 == row || parent2 == parent1);

      int parent3 = -1;
      do {
        parent3 = rand_lim(DIFF_POPULATION_SIZE - 1);
      } while(parent3 == row || parent3 == parent1 || parent3 == parent2);

      int cut_point = rand_lim(NUM_PARAMETERS);

      for(int i = 0; i < NUM_PARAMETERS; i++) {
        if(rand_one() < DIFF_CROSSOVER_RATE || i == cut_point) {
          new_individual[i] = population[parent3][i] + DIFF_WEIGHTING_FACTOR * (population[parent2][i] - population[parent1][i]);
        }
        else {
          new_individual[i] = population[row][i];
        }
      }

      test_chromosome->beta  = population[row][BETA];
      test_chromosome->gamma = population[row][GAMMA];
      test_chromosome->mu    = population[row][MU];

      double p_fitness = chromosome_fitness(test_chromosome, target_S, target_I, target_R);

      test_chromosome->beta  = new_individual[BETA];
      test_chromosome->gamma = new_individual[GAMMA];
      test_chromosome->mu    = new_individual[MU];

      double s_fitness = chromosome_fitness(test_chromosome, target_S, target_I, target_R);

      if(s_fitness < p_fitness) {
        average_fitness += s_fitness / DIFF_POPULATION_SIZE;
        for(int i = 0; i < NUM_PARAMETERS; i++) {
          new_population[row][i] = new_individual[i];
        }

        fitnesses[row] = s_fitness;
      }
      else {
        average_fitness += p_fitness / DIFF_POPULATION_SIZE;
        for(int i = 0; i < NUM_PARAMETERS; i++) {
          new_population[row][i] = population[row][i];
        }

        fitnesses[row] = p_fitness;
      }
    }

    for(int row = 0; row < DIFF_POPULATION_SIZE; row++) {
      for(int col = 0; col < NUM_PARAMETERS; col++) {
        population[row][col] = new_population[row][col];
      }
    }
  }
 
  float best_fitness = -1;
  int best_fitness_index = 0;

  for(int row = 0; row < DIFF_POPULATION_SIZE; row++) {
    if(fitnesses[row] < best_fitness || best_fitness == -1) {
      best_fitness = fitnesses[row];
      best_fitness_index = row;
    }
  }
  
  c->beta  = population[best_fitness_index][BETA];
  c->gamma = population[best_fitness_index][GAMMA];
  c->mu    = population[best_fitness_index][MU];
}
