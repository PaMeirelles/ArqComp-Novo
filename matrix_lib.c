#include "matrix_lib.h"
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <errno.h>
#include <immintrin.h>
#include <pthread.h>

#define NUM_THREADS 8

int aux_scalar(s_matrix * matrix, int inicio, int fim, float scalar){
    if(matrix == NULL){
    return 1;
  }
  __m256 a = _mm256_set1_ps(scalar);
  __m256 b;
  for(int i=inicio; i < fim; i += 8){
    b = _mm256_load_ps(matrix->rows+i);
    b = _mm256_mul_ps(a, b);
    _mm256_store_ps(matrix->rows+i, b);
  }
  return 0;
}

void aux_mm_mult(s_matrix * matrix_a, s_matrix * matrix_b, s_matrix * matrix_c, int linha){
  long counter = 0;
  float * pointer_a = matrix_a->rows;
  float * pointer_b = matrix_b->rows;
  float * pointer_c = matrix_c->rows;
  long wb = matrix_b->width;
  long hb = matrix_b->height;
  long ha = matrix_a->height;
  long size = hb * wb;

  pointer_a += linha * ha;
  pointer_c += linha * ha;

  __m256 a = _mm256_set1_ps(*pointer_a);
  __m256 b;
  __m256 c;
 while(counter < (ha * wb * hb) / NUM_THREADS){
  counter += 8;

  b = _mm256_load_ps(pointer_b);
  c = _mm256_load_ps(pointer_c);
  c = _mm256_fmadd_ps(a, b, c);
  _mm256_store_ps(pointer_c, c);
  pointer_c += 8;
  pointer_b += 8;
  if(counter % wb == 0){
    pointer_a++;
    a = _mm256_set1_ps(*pointer_a);
    pointer_c -= wb;
  }
  if (counter % size == 0){
    pointer_b -= size;
    pointer_c += wb;
  }
 }
}

void * wrapper_mm_mult(void * par){
  p_m * parametros = (p_m*)par;
  aux_mm_mult(parametros->matrix_a,
      parametros->matrix_b,
      parametros->matrix_c, 
      parametros->linha);
    pthread_exit(0);
}

void * wrapper_scalar(void * par){
  p_s * parametros = (p_s*)par;
  aux_scalar(parametros->matrix, parametros->inicio, parametros->fim, parametros->scalar);
  pthread_exit(0);
}

int matrix_matrix_mult(s_matrix *matrix_a, s_matrix * matrix_b, s_matrix * matrix_c){
  pthread_t threads[NUM_THREADS];
  p_m *parametros[NUM_THREADS];
  int i;
  for(i = 0; i < NUM_THREADS; i++){
    parametros[i] = (p_m*)malloc(sizeof(p_m));

    parametros[i]->matrix_a = matrix_a;
    parametros[i]->matrix_b = matrix_b;
    parametros[i]->matrix_c = matrix_c;
    parametros[i]->linha = i * (matrix_a->height / NUM_THREADS);

    pthread_create(&threads[i], NULL, wrapper_mm_mult, (void *)parametros[i]); 
  }

  for(i = 0; i < NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
    free(parametros[i]);
  }
  return 0;
}

int scalar_matrix_mult(float scalar_value, struct matrix *matrix){
  pthread_t threads[NUM_THREADS];
  p_s *parametros[NUM_THREADS];
  int size = matrix->height * matrix->width;
  int i;
  for(i = 0; i < NUM_THREADS; i++){
    parametros[i] = (p_s*)malloc(sizeof(p_s));

    parametros[i]->matrix = matrix;
    parametros[i]->inicio = i * (size/NUM_THREADS);
    parametros[i]->fim = (i+1) * (size/NUM_THREADS);
    parametros[i]->scalar = scalar_value;

    pthread_create(&threads[i], NULL, wrapper_scalar, (void *)parametros[i]); 
  }

  for(i = 0; i < NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
    free(parametros[i]);
  }
  return 0;
}