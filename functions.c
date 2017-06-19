#include <stdlib.h>

#include "functions.h"
#include "global.h"

void shuffle_array_short_int(short int *array, short int n) {
  if (n > 1) {
    size_t i;

    for (i = 0; i < n - 1; i++) {
      size_t j = i + rand() / (RAND_MAX / (n - i) + 1);

      int t = array[j];

      array[j] = array[i];
      array[i] = t;
    }
  }
}

void bitflip(bool *array, short int n) {
  short int rand_index = rand() % n;

  array[rand_index] = !array[rand_index];
}

int yachts_host_counter(bool *array, short int n) {
  int acumulator = 0;

  for (short int i = 0; i < yachts_quantity; i++)
    if (array[i] == 1)
      acumulator++;

  return acumulator;
}
