#include "matrix_lib.h"
#include <stdlib.h>
#include <stdio.h>
#include "timer.h"
#include <errno.h>
#include <immintrin.h>


void print_matrix(s_matrix * matrix, int n){

  int size = matrix->width * matrix->height;
  for(int i=0; i < size; i++){
    if(i == n){
      printf(" ... ");
      break;
    }
    printf("%.2f ", matrix->rows[i]);
    if((i+1) % matrix->width == 0){
      printf("\n");
    }
    }

  printf("\n");

}

void write_matrix(s_matrix * matrix, FILE * f){
  int size = matrix-> width * matrix->height;
  fwrite(matrix->rows, sizeof(float), size, f);
}

void free_matrix(s_matrix * matrix){
  free(matrix->rows);
  free(matrix);
}

void read_matrix(s_matrix * matrix, FILE * f){
  
  int size = matrix-> width * matrix->height;
  
   int s = fread(matrix->rows, sizeof(float), size, f);

  if(s != size){
     printf("Falha na leitura. Apenas %d elementos lidos, %d esperados\n", s, size);
   }
}

void fill_matrix(s_matrix * matrix, int num){
  for(int i=0; i < matrix->width * matrix->height; i++){
    matrix->rows[i] = num;
  }
}

void test(char * string_scalar, char * string_width_a, char * string_height_a, char * string_width_b, char * string_height_b, char * string_num_threads, char * arq1, char * arq2, char * arq3, char * arq4, int (f)(struct matrix *matrix_a, struct matrix * matrix_b, struct matrix * matrix_c)){
  float scalar = atof(string_scalar);
  int width_a = atoi(string_width_a);
  int height_a = atoi(string_height_a);
  int width_b = atoi(string_width_b);
  int height_b = atoi(string_height_b);
  int num_threads = atoi(string_num_threads);


  if(!(width_a % 8 == 0 && height_a % num_threads % 8 == 0)){
    printf("Dimensões inválidas\n");
    exit(1);
  }

  
  struct timeval start, stop, partial;

  FILE * a1 = fopen(arq1, "r");
  FILE * a2 = fopen(arq2, "r");
  FILE * a3 = fopen(arq3, "w");
  FILE * a4 = fopen(arq4, "w");

  s_matrix * matrix_a = aligned_alloc(32, sizeof(s_matrix));
  s_matrix * matrix_b = aligned_alloc(32, sizeof(s_matrix));
  s_matrix * matrix_c = aligned_alloc(32, sizeof(s_matrix));
  
  matrix_a->width = width_a;
  matrix_a->height = height_a;

  matrix_b->width = width_b;
  matrix_b->height = height_b;

  matrix_c->width = width_b;
  matrix_c->height = height_a;
  
  matrix_a->rows = aligned_alloc(32, sizeof(float) * width_a * height_a);
  matrix_b->rows = aligned_alloc(32, sizeof(float) * width_b * height_b);
  matrix_c->rows = aligned_alloc(32, sizeof(float) * width_a * height_b);

  read_matrix(matrix_a, a1);
  read_matrix(matrix_b, a2);

  fill_matrix(matrix_c, 0);

  set_number_threads(num_threads);
  gettimeofday(&start, NULL);
  scalar_matrix_mult(scalar, matrix_a);
  gettimeofday(&partial, NULL);

  printf("Time for scalar mult: %.2fms\n",   
  timedifference_msec(start, partial));
  write_matrix(matrix_a, a3);
  gettimeofday(&partial, NULL);

  f(matrix_a, matrix_b, matrix_c);
  gettimeofday(&stop, NULL);
  printf("Time for matrix mult: %.2fms\n", 
  timedifference_msec(partial, stop));
  printf("Total time: %.2fms\n", timedifference_msec(start, stop));
  print_matrix(matrix_c, 8);
  
  write_matrix(matrix_c, a4);

  free_matrix(matrix_a);
  free_matrix(matrix_b);
  free_matrix(matrix_c);

  fclose(a1);
  fclose(a2);
  fclose(a3);
  fclose(a4);
}

int main(int argc, char * argv[]){
  test(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], matrix_matrix_mult);
  return 0;
}