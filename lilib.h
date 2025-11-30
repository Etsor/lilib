#ifndef LILIB_H
#define LILIB_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI

// Files
int file_write(const char* filename, const char* data);
int file_append(const char* filename, const char* data);
char* file_read(char* filename);

// Random
int rand_i(int min, int max);
double rand_d(double min, double max);
float rand_f(float min, float max);
int rand_bool();
int rand_sign();
double rand_normal(double mean, double stddev);

// Matrix
// Integer
typedef struct Matrix_i_s {
  size_t rows;
  size_t cols;
  int** data;
} Matrix_i_t;

Matrix_i_t matrix_i_alloc(size_t rows, size_t cols);
void matrix_i_free(Matrix_i_t* m);
Matrix_i_t matrix_i_multiply(Matrix_i_t m1, Matrix_i_t m2);
void matrix_i_fill_random(Matrix_i_t* m, int min_el, int max_el);
void matrix_i_fill_normal(Matrix_i_t* m, int mean, int stddev);
void matrix_i_print(Matrix_i_t m);
void matrix_i_pprint(Matrix_i_t m);

// Double
typedef struct Matrix_d_s {
  size_t rows;
  size_t cols;
  double** data;
} Matrix_d_t;

Matrix_d_t matrix_d_alloc(size_t rows, size_t cols);
void matrix_d_free(Matrix_d_t* m);
Matrix_d_t matrix_d_multiply(Matrix_d_t m1, Matrix_d_t m2);
void matrix_d_fill_random(Matrix_d_t* m, double min_el, double max_el);
void matrix_d_fill_normal(Matrix_d_t* m, double mean, double stddev);
void matrix_d_print(Matrix_d_t m);
void matrix_d_pprint(Matrix_d_t m);

// Float
typedef struct Matrix_f_s {
  size_t rows;
  size_t cols;
  float** data;
} Matrix_f_t;

Matrix_f_t matrix_f_alloc(size_t rows, size_t cols);
void matrix_f_free(Matrix_f_t* m);
Matrix_f_t matrix_f_multiply(Matrix_f_t m1, Matrix_f_t m2);
void matrix_f_fill_random(Matrix_f_t* m, float min_el, float max_el);
void matrix_f_fill_normal(Matrix_f_t* m, float mean, float stddev);
void matrix_f_print(Matrix_f_t m);
void matrix_f_pprint(Matrix_f_t m);

#ifdef LILIB_IMPL
// Files
int file_write(const char* filename, const char* data)
{
    FILE* f = fopen(filename, "w");
    if (!f) return 1;

    if (fputs(data, f) == EOF) {
        fclose(f);
        return 2;
    }

    fclose(f);
    return 0;
}

int file_append(const char* filename, const char* data)
{
    FILE* f = fopen(filename, "a");
    if (!f) return 1;

    if (fputs(data, f) == EOF) {
        fclose(f);
        return 2;
    }

    fclose(f);
    return 0;
}


// Random
int rand_i(int min, int max) { return min + rand() % (max - min + 1); }
double rand_d(double min, double max) { return min + ((double)rand() / (double)RAND_MAX) * (max - min); }
float rand_f(float min, float max) { return min + ((float)rand() / (float)RAND_MAX) * (max - min); }
int rand_bool() { return rand() & 1; }
int rand_sign() { return (rand() & 1) ? 1 : -1; }
double rand_normal(double mean, double stddev) {
    // Box-Muller transform
    double u1 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
    double u2 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return z0 * stddev + mean;
}
// Matrix
// Integer
Matrix_i_t matrix_i_alloc(size_t rows, size_t cols)
{
  Matrix_i_t m = { rows, cols, NULL };

  m.data = malloc(rows * sizeof(int*));
  if (!m.data) {
    m.rows = m.cols = 0;
    m.data = NULL;
    return m;
  }

  for (size_t i = 0; i < rows; ++i) {
    m.data[i] = malloc(cols * sizeof(int));
    if (!m.data[i]) {
      for (size_t j = 0; j < i; ++j)
        free(m.data[j]);
      free(m.data);

      m.rows = m.cols = 0;
      m.data = NULL;

      return m;
    }
  }

  return m;
}

void matrix_i_free(Matrix_i_t* m)
{
  if (!m || !m->data) return;

  for (size_t i = 0; i < m->rows; ++i)
    free(m->data[i]);
  free(m->data);

  m->rows = m->cols = 0;
  m->data = NULL;
}

Matrix_i_t matrix_i_multiply(Matrix_i_t m1, Matrix_i_t m2)
{
  Matrix_i_t err = {0, 0, NULL};
  if (!m1.data || !m2.data) return err;
  if (m1.cols != m2.rows) return err;

  Matrix_i_t m = matrix_i_alloc(m1.rows, m2.cols);
  if (!m.data) return err;

  for (size_t row = 0; row < m1.rows; ++row) {
    for (size_t col = 0; col < m2.cols; ++col) {
      int sum = 0;

      for (size_t k = 0; k < m1.cols; ++k)
        sum += m1.data[row][k] * m2.data[k][col];

      m.data[row][col] = sum;
    }
  }

  return m;
}

void matrix_i_fill_random(Matrix_i_t* m, int min_el, int max_el)
{
  for (size_t row = 0; row < m->rows; ++row)
    for (size_t col = 0; col < m->cols; ++col)
      m->data[row][col] = rand_i(min_el, max_el);
}

void matrix_i_fill_normal(Matrix_i_t* m, int mean, int stddev) {
    for (size_t row = 0; row < m->rows; ++row)
        for (size_t col = 0; col < m->cols; ++col)
            m->data[row][col] = (int)lround(rand_normal((int)mean, (int)stddev));
}

void matrix_i_print(Matrix_i_t m)
{
  for (size_t row = 0; row < m.rows; ++row) {
    for (size_t col = 0; col < m.cols; ++col)
      printf("%d ", m.data[row][col]);
    printf("\n");
  }
}

void matrix_i_pprint(Matrix_i_t m)
{
  for (size_t row = 0; row < m.rows; ++row) {
    for (size_t col = 0; col < m.cols; ++col)
      printf("| %10d ", m.data[row][col]);
    printf("|\n");
  }
}

// Double
Matrix_d_t matrix_d_alloc(size_t rows, size_t cols)
{
  Matrix_d_t m = { rows, cols, NULL };

  m.data = malloc(rows * sizeof(double*));
  if (!m.data) {
    m.rows = m.cols = 0;
    m.data = NULL;
    return m;
  }

  for (size_t i = 0; i < rows; ++i) {
    m.data[i] = malloc(cols * sizeof(double));
    if (!m.data[i]) {
      for (size_t j = 0; j < i; ++j)
        free(m.data[j]);
      free(m.data);

      m.rows = m.cols = 0;
      m.data = NULL;

      return m;
    }
  }

  return m;
}

void matrix_d_free(Matrix_d_t* m)
{
  if (!m || !m->data) return;

  for (size_t i = 0; i < m->rows; ++i)
    free(m->data[i]);
  free(m->data);

  m->rows = m->cols = 0;
  m->data = NULL;
}

Matrix_d_t matrix_d_multiply(Matrix_d_t m1, Matrix_d_t m2)
{
  Matrix_d_t err = {0, 0, NULL};
  if (!m1.data || !m2.data) return err;
  if (m1.cols != m2.rows) return err;

  Matrix_d_t m = matrix_d_alloc(m1.rows, m2.cols);
  if (!m.data) return err;

  for (size_t row = 0; row < m1.rows; ++row) {
    for (size_t col = 0; col < m2.cols; ++col) {
      double sum = 0.0;

      for (size_t k = 0; k < m1.cols; ++k)
        sum += m1.data[row][k] * m2.data[k][col];

      m.data[row][col] = sum;
    }
  }

  return m;
}

void matrix_d_fill_random(Matrix_d_t* m, double min_el, double max_el)
{
  for (size_t row = 0; row < m->rows; ++row)
    for (size_t col = 0; col < m->cols; ++col)
      m->data[row][col] = rand_d(min_el, max_el);
}

void matrix_d_fill_normal(Matrix_d_t* m, double mean, double stddev) {
    for (size_t row = 0; row < m->rows; ++row)
        for (size_t col = 0; col < m->cols; ++col)
            m->data[row][col] = rand_normal(mean, stddev);
}

void matrix_d_print(Matrix_d_t m)
{
  for (size_t row = 0; row < m.rows; ++row) {
    for (size_t col = 0; col < m.cols; ++col)
      printf("%f ", m.data[row][col]);
    printf("\n");
  }
}

void matrix_d_pprint(Matrix_d_t m)
{
  for (size_t row = 0; row < m.rows; ++row) {
    for (size_t col = 0; col < m.cols; ++col)
      printf("| %10.3f ", m.data[row][col]);
    printf("|\n");
  }
}

// Float
Matrix_f_t matrix_f_alloc(size_t rows, size_t cols)
{
  Matrix_f_t m = { rows, cols, NULL };

  m.data = malloc(rows * sizeof(float*));
  if (!m.data) {
    m.rows = m.cols = 0;
    m.data = NULL;
    return m;
  }

  for (size_t i = 0; i < rows; ++i) {
    m.data[i] = malloc(cols * sizeof(float));
    if (!m.data[i]) {
      for (size_t j = 0; j < i; ++j)
        free(m.data[j]);
      free(m.data);

      m.rows = m.cols = 0;
      m.data = NULL;

      return m;
    }
  }

  return m;
}

void matrix_f_free(Matrix_f_t* m)
{
  if (!m || !m->data) return;

  for (size_t i = 0; i < m->rows; ++i)
    free(m->data[i]);
  free(m->data);

  m->rows = m->cols = 0;
  m->data = NULL;
}

Matrix_f_t matrix_f_multiply(Matrix_f_t m1, Matrix_f_t m2)
{
  Matrix_f_t err = {0, 0, NULL};
  if (!m1.data || !m2.data) return err;
  if (m1.cols != m2.rows) return err;

  Matrix_f_t m = matrix_f_alloc(m1.rows, m2.cols);
  if (!m.data) return err;

  for (size_t row = 0; row < m1.rows; ++row) {
    for (size_t col = 0; col < m2.cols; ++col) {
      float sum = 0.0;

      for (size_t k = 0; k < m1.cols; ++k)
        sum += m1.data[row][k] * m2.data[k][col];

      m.data[row][col] = sum;
    }
  }

  return m;
}

void matrix_f_fill_random(Matrix_f_t* m, float min_el, float max_el)
{
  for (size_t row = 0; row < m->rows; ++row)
    for (size_t col = 0; col < m->cols; ++col)
      m->data[row][col] = rand_f(min_el, max_el);
}

void matrix_f_fill_normal(Matrix_f_t* m, float mean, float stddev) {
    for (size_t row = 0; row < m->rows; ++row)
        for (size_t col = 0; col < m->cols; ++col)
            m->data[row][col] = (float)rand_normal((float)mean, (float)stddev);
}

void matrix_f_print(Matrix_f_t m) 
{
  for (size_t row = 0; row < m.rows; ++row) {
    for (size_t col = 0; col < m.cols; ++col)
      printf("%f ", m.data[row][col]);
    printf("\n");
  }
}

void matrix_f_pprint(Matrix_f_t m) 
{
  for (size_t row = 0; row < m.rows; ++row) {
    for (size_t col = 0; col < m.cols; ++col)
      printf("| %10.3f ", m.data[row][col]);
    printf("|\n");
  }
}
#endif // LILIB_IMPL

#endif // LILIB_H
