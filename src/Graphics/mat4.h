#ifndef MAT4_H
#define MAT4_H

#include <math.h>

typedef float mat4[16];
typedef float vec3[3];
typedef float vec2[2];

void mat4_Ortho(float *out, float left, float right, float bottom, float top, float zNear, float zFar);

void mat4_Translate2D(float *out, const float *a, float *v);

void mat4_Rotate2D(float *out, float *in, float angle);

void mat4_Scale2D(float *out, float *a, float *v);

void mat4_identity(float *out);

void mat4_clone(float *out, float *a);

void mat4_multiplyM(float *out, float *a, float *b);

void mat4_multiplyMV2(float *out, float *m, float *v);

void vec2_multiplyVM(float *out, float *v, float *m);

void vec2_Rotate2D(float *out, float angle);

#endif