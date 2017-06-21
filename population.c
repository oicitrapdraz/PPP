#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "global.h"
#include "population.h"

// Se tiene que editar la funcion suffle, ya que ahora se haria shuffle de un arreglo en particular de un cromosoma en particular

void sort_population(chrom *population, bool verb) {

  if (verb)
    printf("\n-- REALIZANDO ORDENAMIENTO --\n");

  chrom aux;

  for (short int i = 0; i < pop_size - 1; i++)
    for (short int j = 0; j < pop_size - i - 1; j++)
        if (population[j + 1].unfitness < population[j].unfitness) {
          memcpy(&aux, &population[j + 1], sizeof(chrom));
          memcpy(&population[j + 1], &population[j], sizeof(chrom));
          memcpy(&population[j], &aux, sizeof(chrom));
        }

  for (short int i = 0; i < pop_size; i++) {
    population[i].fit = log(pop_size - i + 1) + 1;
    if (verb)
      printf("El cromosoma %4hu tiene un fitness de: %4hu (no aptitud de %4d) y es ", i, population[i].fit, population[i].unfitness);

    if (verb) {
      if (population[i].factible)
        printf("factible\n");
      else
        printf("infactible\n");
    }
  }
}

void  initialize_population(chrom **population) {
  chrom *tmp_pop = malloc(pop_size * sizeof(chrom));

  // tmp_pop: Lista de cromosomas que conforman nuestra poblacion de soluciones

  for (short int i = 0; i < pop_size; i++) {
    obtain_h(&tmp_pop[i]);
    obtain_yachts_host_indexes(&tmp_pop[i]);
    obtain_x(&tmp_pop[i]);
    obtain_m(&tmp_pop[i]);
    calculate_unfitness(&tmp_pop[i], false);
  }

  (*population) = tmp_pop;
}

void calculate_next_population(chrom **population) {
  float *acum_fit = malloc(pop_size * sizeof(float));
  int acumulator = 0;

  // Calculamos la sumatoria de los fitnes para la seleccion por medio de ruleta

  for (short int i = 0; i < pop_size; i++) {
    acumulator += (*population)[i].fit;
    acum_fit[i] = acumulator;
  }

  // tmp_pop: Sera la poblacion intermedia

  chrom *tmp_pop = (chrom *)malloc(pop_size * sizeof(chrom));

  // Pondremos automaticamente la mejor solucion de la poblacion original en la poblacion intermedia

  deep_copy_chromosome(&tmp_pop[0], &(*population)[0]);

  // Aqui realizamos la seleccion

  for (int i = 1; i < pop_size; i++) {
    short int selector = rand() % acumulator + 1;
    int index = 0;

    while (acum_fit[index] < selector)
      index++;

    deep_copy_chromosome(&tmp_pop[i], &(*population)[index]);
  }

  // En este punto tenemos a la poblacion intermedia en tmp_pop, borramos la poblacion original y a population le asignamos la poblacion intermedia

  free(acum_fit);

  delete_population(&(*population));

  (*population) = tmp_pop;

  for (short int i = 1; i < pop_size; i++)
    if (prob_hill_climbing_1 > rand() % 100)
      hill_climbing_1(&(*population)[i]);

  for (short int i = 1; i < pop_size; i++)
    if (prob_hill_climbing_2 > rand() % 100)
      hill_climbing_2(&(*population)[i]);

  for (short int i = 1; i < pop_size; i++)
    if (prob_hill_climbing_3 > rand() % 100)
      hill_climbing_3(&(*population)[i]);
}

void delete_population(chrom **population) {
  for (short int i = 0; i < pop_size; i++)
    delete_chromosome_yachts_host_indexes(&(*population)[i]);

  free(*population);
  (*population) = NULL;
}

void show_population(chrom *population) {
  for (short int i = 0; i < pop_size; i++) {
    printf("\n--- CROMOSOMA %d ---\n\n", i);
    show_chromosome(&population[i]);
  }
}
