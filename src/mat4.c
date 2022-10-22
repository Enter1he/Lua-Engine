typedef float mat4[16];
typedef float vec3[3];
typedef float vec2[2];

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

  if (a == out) {
    out[12] = a[0] * x + a[4] * y + a[12];
    out[13] = a[1] * x + a[5] * y + a[13];
    out[14] = a[2] * x + a[6] * y + a[14];
    out[15] = a[3] * x + a[7] * y + a[15];
  } else {
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

    out[0] = a00;
    out[1] = a01;
    out[2] = a02;
    out[3] = a03;
    out[4] = a10;
    out[5] = a11;
    out[6] = a12;
    out[7] = a13;
    out[8] = a20;
    out[9] = a21;
    out[10] = a22;
    out[11] = a23;

    out[12] = a00 * x + a10 * y + a[12];
    out[13] = a01 * x + a11 * y + a[13];
    out[14] = a02 * x + a12 * y + a[14];
    out[15] = a03 * x + a13 * y + a[15];
  }

}

void mat4_Scale2D(float *out, float *a, float *v){
  float x = v[0],
    y = v[1];

  out[0] = a[0] * x;
  out[1] = a[1] * x;
  out[2] = a[2] * x;
  out[3] = a[3] * x;
  out[4] = a[4] * y;
  out[5] = a[5] * y;
  out[6] = a[6] * y;
  out[7] = a[7] * y;
  out[12] = a[12];
  out[13] = a[13];
  out[14] = a[14];
  out[15] = a[15];
  
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
  out[0] = a[0];
  out[1] = a[1];
  out[2] = a[2];
  out[3] = a[3];
  out[4] = a[4];
  out[5] = a[5];
  out[6] = a[6];
  out[7] = a[7];
  out[8] = a[8];
  out[9] = a[9];
  out[10] = a[10];
  out[11] = a[11];
  out[12] = a[12];
  out[13] = a[13];
  out[14] = a[14];
  out[15] = a[15];
}