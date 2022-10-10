#include <stdio.h>
struct matrix {
unsigned long int height;
unsigned long int width;
float *rows;
} typedef s_matrix;

struct parametros_mm_mult {
  s_matrix * matrix_a;
  s_matrix * matrix_b;
  s_matrix * matrix_c;
  int linha;
} typedef p_m;

struct parametros_scalar{
  s_matrix * matrix;
  int inicio;
  int fim;
  float scalar;
} typedef p_s;
void set_number_threads(int num_threads);
int scalar_matrix_mult(float scalar_value, struct matrix *matrix);
int matrix_matrix_mult(struct matrix *matrix_a, struct matrix * matrix_b, struct matrix * matrix_c);