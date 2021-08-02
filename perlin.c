#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "perlin.h"
#include "math.h"

float lerp(float a, float b, float f) {
  return (1.0 - f) * a + b * f;
}

float fade(float t) {
 return t * t * t * (t * (t * 6 - 15) + 10);
}

float computeGradientEffect(fvec2 *grid, int tableWidth, ivec2 pos, ivec2 rel, unsigned int unit) {
  ivec2 gridPos = ivec2_add(ivec2_div1(pos, unit), rel);
  fvec2 gridReal = fvec2_mul1(ivec2_as_fvec2(gridPos), unit);

  fvec2 grad = grid[(gridPos.y) * tableWidth + (gridPos.x)];
  fvec2 relativePos = fvec2_sub(ivec2_as_fvec2(pos), gridReal);
  relativePos = fvec2_div1(relativePos, unit);
  
  return fvec2_dot(relativePos, grad);
}

void perlin(size_t width, size_t height, unsigned int unit, float *output) {
  unsigned int tableWidth = (width + unit - 1) / unit + 1; 
  unsigned int tableHeight = (height + unit - 1) / unit + 1; 
  fvec2 *table = malloc(tableHeight * tableWidth * sizeof(fvec2)); 
  assert(table != NULL);
  
  for (unsigned int x = 0; x < tableWidth; x++) {
    for (unsigned int y = 0; y < tableHeight; y++) {
      table[tableWidth * y + x] = fvec2_random();
    }
  }

  for (unsigned int x = 0; x < width; x++) {
    for (unsigned int y = 0; y < height; y++) {
      float x0 = (float)(x % (unit)) / unit;
      float y0 = (float)(y % (unit)) / unit;
      
      ivec2 pos = (ivec2) {.x = x, .y = y}; 
      float n0 = computeGradientEffect(table, tableWidth, pos, (ivec2){.x=0, .y=0}, unit);
      float n1 = computeGradientEffect(table, tableWidth, pos, (ivec2){.x=1, .y=0}, unit);

      float nn = lerp(n0, n1, fade(x0));

      float s0 = computeGradientEffect(table, tableWidth, pos, (ivec2){.x=0, .y=1}, unit);
      float s1 = computeGradientEffect(table, tableWidth, pos, (ivec2){.x=1, .y=1}, unit);

      float ss = lerp(s0, s1, fade(x0));
      float ns = lerp(nn, ss, fade(y0));

      float n = (ns + 1) / 2;
      
      output[y * width + x] = n * n *n; 
    }
  }
}
