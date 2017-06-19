#ifndef GLOBAL_H

#include <stdbool.h>

// Parametros del AE

extern short int pop_size;

extern short int penalization_factor;

extern short int prob_hill_climbing_1;
extern short int prob_hill_climbing_2;
extern short int prob_hill_climbing_3;

short int hc_iterations;

// Parametros del ejercicio

extern short int yachts_quantity;

extern short int periods;

extern short int yachts_capacities[42];
extern short int yachts_crew_size[42];

#endif
