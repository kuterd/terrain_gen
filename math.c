#include <assert.h>
#include "math.h"

void multiplyMatrix(matrix4 a, matrix4 b, matrix4 result) {
    assert((a != result || b != result) && "Output is the same as the input bad things can happen");
    
    for (size_t x = 0; x < 4; x++) {
        for (size_t y = 0; y < 4; y++) {
            float sum = 0;
            // dot product between line and row.
            for (size_t i = 0; i < 4; i++)
                sum += M4CELL(a, i, y) * M4CELL(b, x, i); 
            M4CELL(result, x, y) = sum;
        }
    }
}

matrix4 createIdentity() {
    matrix4 result = M4CALLOC(); 
    M4CELL(result, 0, 0) = 1;
    M4CELL(result, 1, 1) = 1;
    M4CELL(result, 2, 2) = 1;
    M4CELL(result, 3, 3) = 1;

    return result;
}

matrix4 createProjectionMatrix(float fov, float aspect, float far, float near) {
    matrix4 result = M4CALLOC();

    M4CELL(result, 0, 0) = 1 / (aspect * tan(fov / 2));
    M4CELL(result, 1, 1) = 1 / tan(fov / 2); 
    M4CELL(result, 2, 2) = - (far + near) / (far - near); 
    M4CELL(result, 3, 2) = - (2 * far * near) / (far - near); 
    M4CELL(result, 2, 3) = -1; 

    return result;
}

matrix4 createTranslation(float x, float y, float z) {
    matrix4 result = createIdentity(); 

    M4CELL(result, 3, 0) = x; 
    M4CELL(result, 3, 1) = y; 
    M4CELL(result, 3, 2) = z; 

    return result;
}

matrix4 createRotation(float x, float y, float z) {
    matrix4 result = M4CALLOC();
 
    M4CELL(result, 0, 0) = cos(x) * cos(y); 
    M4CELL(result, 1, 0) = cos(x) * sin(y) * sin(z) - sin(x) * cos(z); 
    M4CELL(result, 2, 0) = cos(x) * sin(y) * cos(z) + sin(x) * sin(z); 
    
    M4CELL(result, 0, 1) = sin(x) * cos(y); 
    M4CELL(result, 1, 1) = sin(x) * sin(y) * sin(z) + cos(x) * cos(z); 
    M4CELL(result, 2, 1) = sin(x) * sin(y) * cos(z) - cos(x) * sin(z); 
   
    M4CELL(result, 0, 2) = -sin(y); 
    M4CELL(result, 1, 2) = cos(y) * sin(z); 
    M4CELL(result, 2, 2) = cos(y) * cos(z); 

    M4CELL(result, 3, 3) = 1;

    return result;
}




float float_random() {
  return (float)rand() / (float)RAND_MAX;
}

fvec2 fvec2_div1(fvec2 a, float b) {
  return (fvec2) {.x=a.x / b, .y=a.y / b};
}

fvec2 fvec2_mul1(fvec2 a, float b) {
  return (fvec2) {.x=a.x * b, .y=a.y * b};
}

fvec2 fvec2_sub(fvec2 a, fvec2 b) {
  return (fvec2) {.x=a.x - b.x, .y=a.y - b.y};
}

float fvec2_dot(fvec2 a, fvec2 b) {
  return a.x * b.x + a.y * b.y;
}

float fvec2_length(fvec2 vec) {
  return sqrt(vec.x * vec.x + vec.y * vec.y);
}

fvec2 fvec2_random() {
  fvec2 fvec = (fvec2) {.x = (float_random() * 2 - 1) * 2, .y = (float_random() * 2 - 1) * 2};
  return fvec2_div1(fvec, fvec2_length(fvec)); 
}

ivec2 ivec2_div1(ivec2 a, int div) {
  return (ivec2) {.x=a.x / div, .y=a.y / div};
}

ivec2 ivec2_add(ivec2 a, ivec2 b) {
  return (ivec2) {.x=a.x + b.x, .y=a.y + b.y};
}

fvec3 crossProduct(fvec3 a, fvec3 b) {
   return (fvec3) { a.y * b.z - a.z * b.y,
                    a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x }; 
}


