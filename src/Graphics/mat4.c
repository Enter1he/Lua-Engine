#include "src/Graphics/mat4.h"



void mat4_Ortho(float *out, float left, float right, float bottom, float top, float zNear, float zFar){
  const float lr = 1 / (left - right);
  const float bt = 1 / (bottom - top);
  const float nf = 1 / (zNear - zFar);
  out[0] = -2 * lr;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
  out[4] = 0;
  out[5] = -2 * bt;
  out[6] = 0;
  out[7] = 0;
  out[8] = 0;
  out[9] = 0;
  out[10] = nf;
  out[11] = 0;
  out[12] = (left + right) * lr;
  out[13] = (top + bottom) * bt;
  out[14] = zNear * nf;
  out[15] = 1;
}

void mat4_Translate2D(float *out, const float *a, float *v){
  float x = v[0],
    y = v[1];
  float a00, a01, a02, a03;
  float a10, a11, a12, a13;
  float a20, a21, a22, a23;

  mat4 res;
  
    a00 = a[0];
    a01 = a[1];
    a02 = a[2];
    a03 = a[3];
    a10 = a[4];
    a11 = a[5];
    a12 = a[6];
    a13 = a[7];
    a20 = a[8];
    a21 = a[9];
    a22 = a[10];
    a23 = a[11];

    res[0] = a00;
    res[1] = a01;
    res[2] = a02;
    res[3] = a03;
    res[4] = a10;
    res[5] = a11;
    res[6] = a12;
    res[7] = a13;
    res[8] = a20;
    res[9] = a21;
    res[10] = a22;
    res[11] = a23;

    res[12] = a00 * x + a10 * y + a[12];
    res[13] = a01 * x + a11 * y + a[13];
    res[14] = a02 * x + a12 * y + a[14];
    res[15] = a03 * x + a13 * y + a[15];

  for (int i = 0; i < 16; i++)
    out[i] = res[i];
}

void mat4_Scale2D(float *out, float *a, float *v){
  float x = v[0],
    y = v[1];

  mat4 res;

  res[0] = a[0] * x;
  res[1] = a[1] * x;
  res[2] = a[2] * x;
  res[3] = a[3] * x;
  res[4] = a[4] * y;
  res[5] = a[5] * y;
  res[6] = a[6] * y;
  res[7] = a[7] * y;
  res[12] = a[12];
  res[13] = a[13];
  res[14] = a[14];
  res[15] = a[15];

  for (int i = 0; i < 16; i++)
    out[i] = res[i];
}

#define PI 3.141592653589793238462643383279502884

void mat4_Rotate2D(float *out, float *in, float angle){
  angle = angle * (PI / 180.0);
  float cosA = cos(angle), sinA = sin(angle);
  mat4 n = 
  { cosA, -sinA, 0, 0,
    sinA, cosA,  0, 0,
    0,    0,     1, 0,
    0,    0,     0, 1
  };
  mat4_multiplyM(out, n, in);
}

void mat4_identity(float *out){
  out[0] = 1;
  out[1] = 0;
  out[2] = 0;
  out[3] = 0;
  out[4] = 0;
  out[5] = 1;
  out[6] = 0;
  out[7] = 0;
  out[8] = 0;
  out[9] = 0;
  out[10] = 1;
  out[11] = 0;
  out[12] = 0;
  out[13] = 0;
  out[14] = 0;
  out[15] = 1;
}

void mat4_clone(float *out, float *a){
  if (a == out) return;
  out[0] = a[0];    out[1] = a[1];    out[2] = a[2];    out[3] = a[3];
  out[4] = a[4];    out[5] = a[5];    out[6] = a[6];    out[7] = a[7];
  out[8] = a[8];    out[9] = a[9];    out[10] = a[10];  out[11] = a[11];
  out[12] = a[12];  out[13] = a[13];  out[14] = a[14];  out[15] = a[15];
}

void mat4_multiplyM(float *out, float *a, float *b){
  mat4 res;
  res[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12],
  res[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13],
  res[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14],
  res[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15],
  res[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12],
  res[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13],
  res[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14],
  res[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15],
  res[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12],
  res[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13],
  res[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14],
  res[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15],
  res[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12],
  res[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13],
  res[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14],
  res[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
  for (int i = 0; i < 16; i++)
    out[i] = res[i];
}

void mat4_multiplyMV2(float *out, float *m, float *v){
  mat4 res;
  float x = v[0], y = v[1];
  res[0] = m[0] * x;
  res[1] = m[1] * y;
  res[2] = m[2] * 0;
  res[3] = m[3] * 1;
  res[4] = m[4] * x;
  res[5] = m[5] * y;
  res[6] = m[6] * 0;
  res[7] = m[7] * 1;
  res[8] = m[8] * x;
  res[9] = m[9] * y;
  res[10] = m[10] * 0;
  res[11] = m[11] * 1;
  res[12] = m[12] * x;
  res[13] = m[13] * y;
  res[14] = m[14] * 0;
  res[15] = m[15] * 1;
  for (int i = 0; i < 16; i++)
    out[i] = res[i];
}

void vec2_Rotate2D(float *out, float angle){
  float res[2];
  angle = angle * (PI / 180.0);
  float cosA = cos(angle), sinA = sin(angle);
  float n[16] = 
  { cosA, -sinA, 0, 0,
    sinA, cosA,  0, 0,
    0,    0,     1, 0,
    0,    0,     0, 1
  };
  res[0] = out[0]*cosA-out[1]*sinA;
  res[1] = out[0]*sinA+out[1]*cosA;

  out[0] = res[0];
  out[1] = res[1];
}

void vec2_multiplyVM(float *out, float *v, float *m){
  vec2 res;
  res[0] = v[0]*m[0]+v[1]*m[1]+m[3];
  res[1] = v[0]*m[4]+v[1]*m[5]+m[7];
  for (int i = 0; i < 2; i++)
    out[i] = res[i];
}

#undef PI