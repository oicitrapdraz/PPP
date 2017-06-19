#ifndef CHROMOSOME_H

#include <stdbool.h>

// Estructura del cromosoma (es importante que concuerde con los parametros del ejercicio)

typedef struct chrom {
  // x: Especifica quien visita a quien y en que periodo
  // m: Especifica quien se encontro con quien y en que periodo

  bool x[42][42][6];
  bool m[42][42][6];

  // h: Determina cuales yates son anfitriones
  // yachts_host_quantity: Numero de yates anfitriones.}
  // yachts_host_indexes: Determina el orden de visita que las tripulaciones realizaran

  bool h[42];

  short int yachts_host_quantity;

  short int **yachts_host_indexes;

  // fit: Entre mayor sea este valor quiere decir que sera una mejor solucion candidata comparada con su poblacion
  // unfitness: Determina el grado de quebrantamiento de restricciones y numero de yates anfitriones
  // factible: Determina si este cromosoma tiene una solucion factible o no

  short int fit;
  int unfitness;
  bool factible;
} chrom;

void obtain_h(chrom *chromosome);

void obtain_yachts_host_indexes(chrom *chromosome);

void obtain_x(chrom *chromosome);

void obtain_m(chrom *chromosome);

void calculate_unfitness(chrom *chromosome, bool verb);

void deep_copy_chromosome(chrom *chromosome_1, chrom *chromosome_2);

void show_chromosome(chrom *chromosome);

void delete_chromosome_yachts_host_indexes(chrom *chromosome);

void hill_climbing_1(chrom *chromosome);

void hill_climbing_2(chrom *chromosome);

void hill_climbing_3(chrom *chromosome);

#endif
