#ifndef POPULATION_H

#include <stdbool.h>

#include "chromosome.h"

void sort_population(chrom *population, bool verb);

void initialize_population(chrom **population);

void calculate_next_population(chrom **population);

void delete_population(chrom **population);

void show_population(chrom *population);

#endif
