#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "functions.h"
#include "chromosome.h"

// Instancia correctamente el arreglo h con una cantidad de yates anfitriones igual o superior al numero de periodos

void obtain_h(chrom *chromosome) {
  do {
    for (short int j = 0; j < yachts_quantity; j++) {
      int bit = rand() % 2;

      chromosome->h[j] = bit;
    }

    chromosome->yachts_host_quantity = yachts_host_counter(chromosome->h, yachts_quantity);
  } while ((chromosome->yachts_host_quantity < periods) || (chromosome->yachts_host_quantity == yachts_quantity));

  chromosome->yachts_host_indexes = NULL;
}

// Instancia correctamente el arreglo yachts_host_indexes que contiene los indices de los yates host

void obtain_yachts_host_indexes(chrom *chromosome) {
  if (chromosome->yachts_host_indexes == NULL) {
    chromosome->yachts_host_indexes = malloc(yachts_quantity * sizeof(short int *));

    for (short int i = 0; i < yachts_quantity; i++)
      chromosome->yachts_host_indexes[i] = malloc(periods * sizeof(short int));

    for (short int i = 0; i < yachts_quantity; i++)
      get_rand_host_index(chromosome, chromosome->yachts_host_indexes[i]);
  }
}

// Instancia correctamente la matriz x dependiendo de los valores del arreglo h

void obtain_x(chrom *chromosome) {
  for (short int i = 0; i < yachts_quantity; i++)
    for (short int j = 0; j < yachts_quantity; j++)
        for (short int k = 0; k < periods; k++)
          chromosome->x[i][j][k] = 0;

  for (short int j = 0; j < yachts_quantity; j++)
    for (short int k = 0; k < periods; k++) {
      if (chromosome->h[j] != 1 && k < periods) {
        short int yacht_host_index = chromosome->yachts_host_indexes[j][k];

        chromosome->x[yacht_host_index][j][k] = 1;
      }
    }
}

// Instancia correctamente la matriz m dependiendo de los valores de la matriz x

void obtain_m(chrom *chromosome) {
  for (short int i = 0; i < yachts_quantity; i++)
    for (short int j = 0; j < yachts_quantity; j++)
      for (short int k = 0; k < periods; k++)
        chromosome->m[i][j][k] = 0;

  for (short int i = 0; i < yachts_quantity; i++)
    for (short int j = 0; j < yachts_quantity; j++)
      for (short int l = 0; l < j; l++)
        for (short int k = 0; k < periods; k++)
          if (chromosome->x[i][j][k] && chromosome->x[i][l][k]) {
            chromosome->m[j][l][k] += 1;
            chromosome->m[l][j][k] += 1;
          }
}

void calculate_unfitness(chrom *chromosome, bool verb) {
  int visits_to_nonhost_yatch = 0, exceeded_capacity = 0, idle_or_host_visiting = 0, visits_same_host = 0, meet_same_people = 0;

  if (verb)
    printf("\nSe tienen %hu yates anfitriones", chromosome->yachts_host_quantity);

  if (verb) {
    printf("Los indices de los yates anfitriones son:\n");

    for (short int i = 0; i < yachts_quantity; i++)
      printf("%d ", chromosome->h[i]);

    printf("}\n");
  }

  // -- RESTRICCIONES --

  // Un yate solo puede ser visitado si es anfitrion

  for (short int i = 0; i < yachts_quantity; i++)
    for (short int j = 0; j < yachts_quantity; j++)
      for (short int k = 0; k < periods; k++)
        if ((chromosome->x[i][j][k] > chromosome->h[i]) && (i != j))
          visits_to_nonhost_yatch++;

  if (visits_to_nonhost_yatch > 0 && verb)
    printf("\nEn total hubieron %d visitas a yates no anfitriones", visits_to_nonhost_yatch);

  // La capacidad de un yate anfitrion no puede ser excedido

  for (short int i = 0; i < yachts_quantity; i++)
    if (chromosome->h[i])
      for (short int k = 0; k < periods; k++) {
        int acumulated_incoming_crew = 0;
        for (short int j = 0; j < yachts_quantity; j++)
          if ((i != j) && chromosome->h[i])
            acumulated_incoming_crew += yachts_crew_size[j] * chromosome->x[i][j][k];

        if (acumulated_incoming_crew > yachts_capacities[i] - yachts_crew_size[i])
          exceeded_capacity += acumulated_incoming_crew - (yachts_capacities[i] - yachts_crew_size[i]);
      }

  if (exceeded_capacity > 0 && verb)
    printf("\nEn total existieron %d sobre excesos respecto a las capacidades de los yates", exceeded_capacity);

  // Cada tripulacion debe ser anfitrion o huesped en un yate anfitrion en cada periodo de tiempo (no idle), pero un anfitrion tampoco debe visitar otros yates

  for (short int j = 0; j < yachts_quantity; j++)
    for (short int k = 0; k < periods; k++) {
      int acumulated_visits = 0;
      for (short int i = 0; i < yachts_quantity; i++)
        acumulated_visits += chromosome->x[i][j][k];

      if ((chromosome->h[j] + acumulated_visits) != 1)
        idle_or_host_visiting += 1;
    }

  if (idle_or_host_visiting > 0 && verb) {
    printf("\nEn total existieron %d casos en que una tripulacion: ", idle_or_host_visiting);
    printf("\n - Estubo idle");
    printf("\n - Visito a mas de un yate");
    printf("\n - Visito a algun yate siendo host");
  }

  // Un huesped no puede visitar a un anfitrion más de una vez

  for (short int i = 0; i < yachts_quantity; i++)
    for (short int j = 0; j < yachts_quantity; j++) {
      int acumulated_visits = 0;
      for (short int k = 0; k < periods; k++)
        acumulated_visits += chromosome->x[i][j][k];

      if (acumulated_visits > 1)
        visits_same_host += acumulated_visits - 1;
    }

  if (visits_same_host > 0 && verb)
    printf("\nExistieron %d casos en que tripulaciones se reencontraron con los mismos yates anfitionres", visits_same_host);

  // Cada par de tripulaciones a lo mas se pueden encontrar 1 vez

  for (short int i = 0; i < yachts_quantity; i++)
    for (short int j = 0; j < i; j++) {
      int acumulated_meets = 0;
      for (short int k = 0; k < periods; k++)
        acumulated_meets += chromosome->m[i][j][k];

      if (acumulated_meets > 1)
        meet_same_people += acumulated_meets - 1;
    }

  if (meet_same_people > 0 && verb)
    printf("\nExistieron %d reencuentros que no debieron haber ocurrido entre yates no anfitriones", meet_same_people);

  chromosome->unfitness = penalization_factor * (visits_to_nonhost_yatch + exceeded_capacity + idle_or_host_visiting + visits_same_host + meet_same_people) + chromosome->yachts_host_quantity;

  if (chromosome->unfitness > chromosome->yachts_host_quantity)
    chromosome->factible = false;
  else
    chromosome->factible = true;

  if (verb)
    printf("\n\nOrden de no aptitud %d!\n", chromosome->unfitness);
}

void deep_copy_chromosome(chrom *chromosome_1, chrom *chromosome_2) {
  memcpy(chromosome_1, chromosome_2, sizeof(chrom));

  chromosome_1->yachts_host_indexes = malloc(yachts_quantity * sizeof(short int *));

  for (short int i = 0; i < yachts_quantity; i++)
    chromosome_1->yachts_host_indexes[i] = malloc(periods * sizeof(short int));

  for (short int i = 0; i < yachts_quantity; i++)
    memcpy(chromosome_1->yachts_host_indexes[i], chromosome_2->yachts_host_indexes[i], periods * sizeof(short int));
}

void show_chromosome(chrom *chromosome) {
  FILE *fp = fopen("results", "w");

  int visits_to_nonhost_yatch = 0, exceeded_capacity = 0, idle_or_host_visiting = 0, visits_same_host = 0, meet_same_people = 0;

  fprintf(fp, "Se tienen %hu yates anfitriones\n\n", chromosome->yachts_host_quantity);

  fprintf(fp, "Distribucion de yates anfitiones { ");

  for (short int i = 0; i < yachts_quantity; i++)
    fprintf(fp, "%d ", chromosome->h[i]);

  fputs("}\n", fp);

  fputs("Los indices de los yates anfitriones son:\n", fp);

  for (short int i = 0; i < yachts_quantity; i++) {
    if (chromosome->h[i] != 1) {
      fprintf(fp, "- Para el yate %2hu: { ", i);
      for (short int j = 0; j < periods; j++)
        fprintf(fp, "%2hu ", chromosome->yachts_host_indexes[i][j]);
      fputs("}\n", fp);
    }
  }

  for (short int k = 0; k < periods; k++) {
    fprintf(fp, "\n\n- EN EL PERIODO: %d\n\n", k);
    for (short int j = 0; j < yachts_quantity; j++)
      for (short int i = 0; i < yachts_quantity; i++)
        if(chromosome->x[i][j][k] == 1)
          fprintf(fp, "El yate %2hu visito al yate %2hu\n", j, i);

    for (short int i = 0; i < yachts_quantity; i++)
      for (short int j = 0; j < i; j++)
        if(chromosome->m[i][j][k] == 1)
          fprintf(fp, "\nTripulantes de los yates %2hu y %2hu se encontraron", i, j);
  }

  fputs("\n", fp);

  // Un yate solo puede ser visitado si es anfitrion

  for (short int i = 0; i < yachts_quantity; i++)
    for (short int j = 0; j < yachts_quantity; j++)
      for (short int k = 0; k < periods; k++)
        if ((chromosome->x[i][j][k] > chromosome->h[i]) && (i != j))
          visits_to_nonhost_yatch++;

  if (visits_to_nonhost_yatch > 0)
    fprintf(fp, "\nEn total hubieron %d visitas a yates no anfitriones", visits_to_nonhost_yatch);

  // La capacidad de un yate anfitrion no puede ser excedido

  for (short int i = 0; i < yachts_quantity; i++)
    if (chromosome->h[i])
      for (short int k = 0; k < periods; k++) {
        int acumulated_incoming_crew = 0;
        for (short int j = 0; j < yachts_quantity; j++)
          if ((i != j) && chromosome->h[i])
            acumulated_incoming_crew += yachts_crew_size[j] * chromosome->x[i][j][k];

        if (acumulated_incoming_crew > yachts_capacities[i] - yachts_crew_size[i])
          exceeded_capacity += acumulated_incoming_crew - (yachts_capacities[i] - yachts_crew_size[i]);
      }

  if (exceeded_capacity > 0)
    fprintf(fp, "\nEn total existieron %d sobre excesos respecto a las capacidades de los yates", exceeded_capacity);

  // Cada tripulacion debe ser anfitrion o huesped en un yate anfitrion en cada periodo de tiempo (no idle), pero un anfitrion tampoco debe visitar otros yates

  for (short int j = 0; j < yachts_quantity; j++)
    for (short int k = 0; k < periods; k++) {
      int acumulated_visits = 0;
      for (short int i = 0; i < yachts_quantity; i++)
        acumulated_visits += chromosome->x[i][j][k];

      if ((chromosome->h[j] + acumulated_visits) != 1)
        idle_or_host_visiting += 1;
    }

  if (idle_or_host_visiting > 0) {
    fprintf(fp, "\nEn total existieron %d casos en que una tripulacion: ", idle_or_host_visiting);
    fputs("\n - Estubo idle", fp);
    fputs("\n - Visito a mas de un yate", fp);
    fputs("\n - Visito a algun yate siendo host", fp);
  }

  // Un huesped no puede visitar a un anfitrion más de una vez

  for (short int i = 0; i < yachts_quantity; i++)
    for (short int j = 0; j < yachts_quantity; j++) {
      int acumulated_visits = 0;
      for (short int k = 0; k < periods; k++)
        acumulated_visits += chromosome->x[i][j][k];

      if (acumulated_visits > 1)
        visits_same_host += acumulated_visits - 1;
    }

  if (visits_same_host > 0)
    fprintf(fp, "\nExistieron %d casos en que tripulaciones se reencontraron con los mismos yates anfitionres", visits_same_host);

  // Cada par de tripulaciones a lo mas se pueden encontrar 1 vez

  for (short int i = 0; i < yachts_quantity; i++)
    for (short int j = 0; j < i; j++) {
      int acumulated_meets = 0;
      for (short int k = 0; k < periods; k++)
        acumulated_meets += chromosome->m[i][j][k];

      if (acumulated_meets > 1)
        meet_same_people += acumulated_meets - 1;
    }

  if (meet_same_people > 0)
    fprintf(fp, "\nExistieron %d reencuentros que no debieron haber ocurrido entre yates no anfitriones", meet_same_people);

  fprintf(fp, "\n\nUnfitness es: %d\n", chromosome->unfitness);

  fputs("\nLa solucion es ", fp);

  if (chromosome->factible)
    fputs("factible\n", fp);
  else
    fputs("infactible\n", fp);

  fclose(fp);
}

void delete_chromosome_yachts_host_indexes(chrom *chromosome) {
  for (short int i = 0; i < yachts_quantity; i++)
    free(chromosome->yachts_host_indexes[i]);

  free(chromosome->yachts_host_indexes);

  chromosome->yachts_host_indexes = NULL;
}

// Alguna mejora con movimiento swap sobre el orden de visita de los yates anfitriones (acepta soluciones iguales tambien) - Explotacion

void hill_climbing_1(chrom *chromosome) {
  int iterations = 0;
  int actual_unfitness = chromosome->unfitness;

  do {
    int rand_index;
    do {
      rand_index = rand() % yachts_quantity;
    } while(chromosome->h[rand_index] == 1);

    short int *yachts_host_index_original = malloc(periods * sizeof(short int));

    memcpy(yachts_host_index_original, chromosome->yachts_host_indexes[rand_index], periods * sizeof(short int));

    shuffle_array_short_int(chromosome->yachts_host_indexes[rand_index], periods);
    obtain_x(chromosome);
    obtain_m(chromosome);
    calculate_unfitness(chromosome, false);

    // Regresamos al orden original de visitas de yates anfitriones

    if ((chromosome->unfitness > actual_unfitness) && (iterations <= hc_iterations))
      memcpy(chromosome->yachts_host_indexes[rand_index], yachts_host_index_original, periods * sizeof(short int));
    else if ((chromosome->unfitness > actual_unfitness) && (iterations > hc_iterations)) {
      memcpy(chromosome->yachts_host_indexes[rand_index], yachts_host_index_original, periods * sizeof(short int));
      obtain_x(chromosome);
      obtain_m(chromosome);
      calculate_unfitness(chromosome, false);
    }

    free(yachts_host_index_original);

    iterations++;
  } while(chromosome->unfitness > actual_unfitness);
}

// Alguna mejora con movimiento bit-flip sobre la configuracion de yates anfitriones (acepta soluciones iguales tambien) - Exploracion

void hill_climbing_2(chrom *chromosome) {
  int iterations = 0;
  int actual_unfitness = chromosome->unfitness;
  bool *host_configuration = malloc(yachts_quantity * sizeof(bool));
  short int **yachts_host_indexes_configuration = NULL;

  yachts_host_indexes_configuration = malloc(yachts_quantity * sizeof(short int *));

  for (short int i = 0; i < yachts_quantity; i++)
    yachts_host_indexes_configuration[i] = malloc(periods * sizeof(short int));

  for (short int i = 0; i < yachts_quantity; i++)
    memcpy(yachts_host_indexes_configuration[i], chromosome->yachts_host_indexes[i], periods * sizeof(short int));

  // host_configuration contendra la configuracion original de yates anfitriones, posteriormente lo uso para que se usen vecinos de la configuracion inicial nada mas y no que se esten haciendo bitflip varias veces sobre el mismo arreglo

  memcpy(host_configuration, chromosome->h, yachts_quantity * sizeof(bool));

  do {
    do {
      memcpy(chromosome->h, host_configuration, yachts_quantity * sizeof(bool));
      bitflip(chromosome->h, yachts_quantity);
      chromosome->yachts_host_quantity = yachts_host_counter(chromosome->h, yachts_quantity);
    } while ((chromosome->yachts_host_quantity < periods) || (chromosome->yachts_host_quantity == yachts_quantity));

    delete_chromosome_yachts_host_indexes(chromosome);
    obtain_yachts_host_indexes(chromosome);
    obtain_x(chromosome);
    obtain_m(chromosome);
    calculate_unfitness(chromosome, false);

    if ((chromosome->unfitness > actual_unfitness) && (iterations > hc_iterations)) {
      memcpy(chromosome->h, host_configuration, yachts_quantity * sizeof(bool));

      chromosome->yachts_host_quantity = yachts_host_counter(chromosome->h, yachts_quantity);

      delete_chromosome_yachts_host_indexes(chromosome);

      chromosome->yachts_host_indexes = malloc(yachts_quantity * sizeof(short int *));

      for (short int i = 0; i < yachts_quantity; i++)
        chromosome->yachts_host_indexes[i] = malloc(periods * sizeof(short int));

      for (short int i = 0; i < yachts_quantity; i++)
        memcpy(chromosome->yachts_host_indexes[i], yachts_host_indexes_configuration[i], periods * sizeof(short int));

      obtain_x(chromosome);
      obtain_m(chromosome);
      calculate_unfitness(chromosome, false);
    }

    iterations++;
  } while(chromosome->unfitness > actual_unfitness);

  for (short int i = 0; i < yachts_quantity; i++)
    free(yachts_host_indexes_configuration[i]);

  free(yachts_host_indexes_configuration);

  free(host_configuration);
}

// Movimiento bit-flip sobre la configuracion de yates anfitriones (acepta cualquier solucion) - Exploracion

void hill_climbing_3(chrom *chromosome)  {
  bool *host_configuration = malloc(yachts_quantity * sizeof(bool));

  memcpy(host_configuration, chromosome->h, yachts_quantity * sizeof(bool));

  delete_chromosome_yachts_host_indexes(chromosome);

  do {
    memcpy(chromosome->h, host_configuration, yachts_quantity * sizeof(bool));
    bitflip(chromosome->h, yachts_quantity);
    chromosome->yachts_host_quantity = yachts_host_counter(chromosome->h, yachts_quantity);
  } while ((chromosome->yachts_host_quantity < periods) || (chromosome->yachts_host_quantity == yachts_quantity));

  obtain_yachts_host_indexes(chromosome);
  obtain_x(chromosome);
  obtain_m(chromosome);
  calculate_unfitness(chromosome, false);

  free(host_configuration);
}

void get_rand_host_index(chrom *chromosome, short int *array) {
  short int *tmp_array = malloc(chromosome->yachts_host_quantity * sizeof(short int));
  bool *added = malloc(chromosome->yachts_host_quantity * sizeof(bool));

  short int actual_index = 0;

  for (short int i = 0; (i < yachts_quantity) && (actual_index < chromosome->yachts_host_quantity); i++)
    if (chromosome->h[i] == 1) {
      tmp_array[actual_index] = i;
      added[actual_index] = false;
      actual_index++;
    }

  for (short int i = 0; i < periods; i++) {
    short int index = rand() % chromosome->yachts_host_quantity;

    if (added[index] == false) {
      array[i] = tmp_array[index];
      added[index] = true;
    } else
      i--;
  }

  free(tmp_array);
  free(added);
}
