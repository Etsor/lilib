#ifndef LILIB_H
#define LILIB_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI

// Files
int   file_write(const char* filename, const char* data);
int   file_append(const char* filename, const char* data);
char* file_read(const char* filename);

// Random
int    rand_i(int min, int max);
double rand_d(double min, double max);
float  rand_f(float min, float max);
int    rand_bool(void);
int    rand_sign(void);
double rand_normal(double mean, double stddev);

// Matrix
#define DECLARE_MATRIX(T, S) \
typedef struct { \
    size_t rows; \
    size_t cols; \
    T**    data; \
} Matrix_##S; \
\
Matrix_##S matrix_##S##_alloc(size_t rows, size_t cols); \
void       matrix_##S##_free(Matrix_##S* m); \
Matrix_##S matrix_##S##_multiply(Matrix_##S m1, Matrix_##S m2); \
void       matrix_##S##_fill_random(Matrix_##S* m, T min_el, T max_el); \
void       matrix_##S##_print(Matrix_##S m); \
void       matrix_##S##_pprint(Matrix_##S m);

DECLARE_MATRIX(int, i)
DECLARE_MATRIX(double, d)
DECLARE_MATRIX(float, f)

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

char* file_read(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    if (size < 0) {
        fclose(f);
        return NULL;
    }

    rewind(f);

    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    size_t read = fread(buffer, 1, size, f);
    fclose(f);

    if (read != size) {
        free(buffer);
        return NULL;
    }

    buffer[size] = '\0';

    return buffer;
}

// Random
int rand_i(int min, int max) { return min + rand() % (max - min + 1); }
double rand_d(double min, double max) { return min + ((double)rand() / (double)RAND_MAX) * (max - min); }
float rand_f(float min, float max) { return min + ((float)rand() / (float)RAND_MAX) * (max - min); }
int rand_bool(void) { return rand() > RAND_MAX / 2; }
int rand_sign(void) { return (rand() & 1) ? 1 : -1; }
double rand_normal(double mean, double stddev) {
    // Box-Muller transform
    double u1 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
    double u2 = ((double)rand() + 1.0) / ((double)RAND_MAX + 1.0);
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);

    return z0 * stddev + mean;
}

// Matrix
#define IMPLEMENT_MATRIX(T, S, FMT, PFMT, R_FUNC) \
Matrix_##S matrix_##S##_alloc(size_t rows, size_t cols) { \
    Matrix_##S m = { rows, cols, malloc(rows * sizeof(T*)) }; \
    if (!m.data) return m; \
    for (size_t i = 0; i < rows; ++i) { \
        m.data[i] = malloc(cols * sizeof(T)); \
        if (!m.data[i]) { \
            while(i > 0) free(m.data[--i]); \
            free(m.data); \
            m.data = NULL; \
            return m; } \
    } \
    \
    return m; \
} \
\
void matrix_##S##_free(Matrix_##S* m) { \
    if (!m || !m->data) return; \
    for (size_t i = 0; i < m->rows; ++i) free(m->data[i]); \
        free(m->data); m->data = NULL; \
} \
\
Matrix_##S matrix_##S##_multiply(Matrix_##S m1, Matrix_##S m2) { \
    if (m1.cols != m2.rows) return (Matrix_##S){0,0,NULL}; \
    Matrix_##S res = matrix_##S##_alloc(m1.rows, m2.cols); \
    for (size_t i = 0; i < m1.rows; ++i) { \
        for (size_t j = 0; j < m2.cols; ++j) { \
            T sum = 0; \
            for (size_t k = 0; k < m1.cols; ++k) sum += m1.data[i][k] * m2.data[k][j]; \
            res.data[i][j] = sum; \
        } \
    } \
    \
    return res; \
} \
\
void matrix_##S##_fill_random(Matrix_##S* m, T min_el, T max_el) { \
    for (size_t r = 0; r < m->rows; ++r) \
        for (size_t c = 0; c < m->cols; ++c) m->data[r][c] = R_FUNC(min_el, max_el); \
} \
\
void matrix_##S##_print(Matrix_##S m) { \
    for (size_t r = 0; r < m.rows; ++r) { \
        for (size_t c = 0; c < m.cols; ++c) printf(FMT " ", m.data[r][c]); \
        printf("\n"); \
    } \
} \
\
void matrix_##S##_pprint(Matrix_##S m) { \
    for (size_t r = 0; r < m.rows; ++r) { \
        for (size_t c = 0; c < m.cols; ++c) printf("| " PFMT " ", m.data[r][c]); \
        printf("|\n"); \
    } \
}

IMPLEMENT_MATRIX(int,    i, "%d", "%10d", rand_i)
IMPLEMENT_MATRIX(double, d, "%f", "%10f", rand_d)
IMPLEMENT_MATRIX(float,  f, "%f", "%10f", rand_f)

#endif // LILIB_IMPL

#endif // LILIB_H
