#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "population.h"

// Parametros del AE

short int pop_size = 60;

short int penalization_factor = 1;

short int prob_hill_climbing_1 = 70;
short int prob_hill_climbing_2 = 30;
short int prob_hill_climbing_3 = 55;

short int hc_iterations = 3;

// Parametros del ejercicio

short int yachts_quantity = 42;

short int periods = 6;

short int yachts_capacities[42] = {6, 8, 12 , 12, 12, 12, 12, 10, 10, 10, 10, 10, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 9, 0, 0, 0};
short int yachts_crew_size[42] = {2, 2, 2, 2, 4, 4, 4, 1, 2, 2, 2, 3, 4, 2, 3, 6, 2, 2, 4, 2, 4, 5, 4, 4, 2, 2, 4, 5, 2, 4, 2, 2, 2, 2, 2, 2, 4, 5, 7, 2, 3, 4};

// Inicio del programa

int main() {
  clock_t start, end;
  double cpu_time_used;

  int best_solutiion;
  chrom *population = NULL;

  srand(time(0));

  start = clock();

  if (pop_size > 0) {
    initialize_population(&population);

    sort_population(population, false);

    best_solutiion = population[0].unfitness;

    show_chromosome(&population[0]);

    while (population[0].unfitness > periods) {
      calculate_next_population(&population);
      sort_population(population, false);

      if (population[0].unfitness < best_solutiion) {
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

        printf("\n-- SE ENCONTRO UNA MEJOR SOLUCION CON %d YATES ANFITRIONES EN %f SEGUNDOS --\n", population[0].yachts_host_quantity, cpu_time_used);
        show_chromosome(&population[0]);
        best_solutiion = population[0].unfitness;
      }
    }

    delete_population(&population);
  }

  return 0;
}
