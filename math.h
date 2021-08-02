#ifndef MATH_H
#define MATH_H

#include <math.h>
#include <stdlib.h>

#define RAD(angle) ((M_PI / 180.0) * (angle))

// Column major matrix
typedef float* matrix4;

#define M4CELL(m, x, y) m[(x) * 4 + (y)]
#define M4CALLOC() calloc(4 * 4 * sizeof(float), 1)

void multiplyMatrix(matrix4 a, matrix4 b, matrix4 result);

matrix4 createIdentity();

matrix4 createProjectionMatrix(float fov, float aspect, float far, float near);

matrix4 createTranslation(float x, float y, float z);

matrix4 createRotation(float x, float y, float z);

typedef struct {
    union {
        struct {
            float x, y, z;
        };
        float comp[3]; 
    };
} fvec3;

typedef struct {
  float x, y;
} fvec2;


float float_random();

fvec2 fvec2_div1(fvec2 a, float b);

fvec2 fvec2_mul1(fvec2 a, float b);

fvec2 fvec2_sub(fvec2 a, fvec2 b);

float fvec2_dot(fvec2 a, fvec2 b);

float fvec2_length(fvec2 vec);

fvec2 fvec2_random();

typedef struct {
  int x, y;
} ivec2;

ivec2 ivec2_div1(ivec2 a, int div);

ivec2 ivec2_add(ivec2 a, ivec2 b);

#define ivec2_as_fvec2(a) ((fvec2) {.x=(float)(a).x, .y=(float)(a).y}) 

fvec3 crossProduct(fvec3 a, fvec3 b);

#endif
