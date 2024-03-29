#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define NETWORK_SIZE 10000
#define INITIAL_INFECTED 1

#define DAYS_INFECTIOUS 3
#define DAYS_LATENT 1
#define DAYS_RECOVERED 60

#define REPITITIONS 1

#define R0 3

#define RNOT_SCHEME 1

#define SIMULATION_LENGTH 1865

#define WRITE_BUFFER_SIZE 5000

double T = 0.001;
double R_not = 1;
double X;
int k;

#include "vertex_node.c"
#include "stack.c"
#include "queue.c"
#include "node_pool.c"
#include "vertex.c"
#include "graph.c"

int main(int argc, char *argv[])
{
	srand(time(NULL));

	float T_low, T_high, T_step;
	float R_not_low, R_not_high, R_not_step;

	if(RNOT_SCHEME == 1)
	{
		if(argc != 4)
		{
			printf("Usage: networks R0_low R0_high R0_step\n");
			return 0;
		}

		// Scan in parameters
		sscanf(argv[1], "%f", &R_not_low);
		sscanf(argv[2], "%f", &R_not_high);
		sscanf(argv[3], "%f", &R_not_step);

	}
	else
	{
		if(argc != 4)
		{
			printf("Usage: networks T_low T_high T_step\n");
			return 0;
		}

		// Scan in parameters
		sscanf(argv[1], "%f", &T_low);
		sscanf(argv[2], "%f", &T_high);
		sscanf(argv[3], "%f", &T_step);
	}


	graph *g = graph_create();
	
	char file[100];

	int day;
	double average_times_sick;

	FILE *stats;

	float low, high, step;

	if(RNOT_SCHEME == 1)
	{
		low = R_not_low;
		high = R_not_high;
		step = R_not_step;
	}
	else
	{
		low = T_low;
		high = T_high;
		step = T_step;
	}

	float data[WRITE_BUFFER_SIZE][8];

	int max_infected = 0;
	int peak = 0;
	int infected = 0;

	for(X = low; X < high; X += step)
	{
		snprintf(file, 100, "data/stats-%f.csv", X);
		printf("%s\n", file);
		stats = graph_open_stats(file);	

		int counter = 0;

		for(k = 2; k < 40; k++)
		{
			for(double p = 0.0; p <= 1; p += 0.01)
			{
				printf("k: %i, p: %f\n", k, p);
				for(int simulation = 0; simulation < REPITITIONS; simulation++)
				{
					graph_reset(g);

					graph_circle(g, k);
					graph_rewire(g, p);
					graph_init_infected(g);
						
					peak = 0;
					day = 0;
					while(day < SIMULATION_LENGTH && graph_has_infectious(g) == true)
					{
						//snprintf(file, 100, "graph-data/graph%i.pajek", day);
						//graph_write_pajek(g, &file[0]);
						
						graph_advance(g, day);

						if(g->infectious != NULL)
						{
							infected = g->infectious->length;
							if(infected > peak)
							{
								peak = infected;
							}
						}

						day++;
					}

					average_times_sick = graph_average_times_sick(g);

					//graph_write_stats(g, stats, X, p, k, simulation, day, average_times_sick);

					
					data[counter][0] = X;
					data[counter][1] = p;
					data[counter][2] = k;
					data[counter][3] = simulation;
					data[counter][4] = day;
					data[counter][5] = average_times_sick;
					data[counter][6] = graph_max_infected(g);
					data[counter][7] = peak;

					counter++;

					if(counter == WRITE_BUFFER_SIZE) 
					{
						for(int i = 0; i < WRITE_BUFFER_SIZE; i++)
						{
							graph_write_stats(g, stats, data[i][0], data[i][1], data[i][2], data[i][3], data[i][4], data[i][5], data[i][6], data[i][7]);
						}

						counter = 0;
					}
				}

				//printf("%i %f\n", k, p);
			}
		}

		for(int i = 0; i < counter; i++)
		{
			graph_write_stats(g, stats, data[i][0], data[i][1], data[i][2], data[i][3], data[i][4], data[i][5], data[i][6], data[i][7]);
		}

		fclose(stats);
	}

	return 0;
}
