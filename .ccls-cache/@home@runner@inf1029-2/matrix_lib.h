#include <stdio.h>
struct matrix {
unsigned long int height;
unsigned long int width;
float *rows;
} typedef s_matrix;

struct parametros {
  s_matrix * matrix_a;
  s_matrix * matrix_b;
  s_matrix * matrix_c;
  int linha;
} typedef p;

int scalar_matrix_mult(float scalar_value, struct matrix *matrix);
int matrix_matrix_mult(struct matrix *matrix_a, struct matrix * matrix_b, struct matrix * matrix_c);