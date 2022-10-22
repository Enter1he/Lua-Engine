

#include <stdlib.h>
#include <stdio.h>

#include "include/im.h"
#include "include/im_image.h"
#include "include/im_convert.h"
#include "include/iup.h"          
#include "include/iupgl.h"



#ifdef WIN32
  #include <windows.h>
#endif

#include <GL/gl.h>
// #define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>

PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;


#define glExt_cast(name, type) name = (type) wglGetProcAddress(#name)

GLuint vbo, ubo, cbo, vao, tex;
GLdouble texcoords[] = {
  0.0, 1.0,
  1.0, 1.0, 
  0.0, 0.0,
  1.0, 0.0, 
  
};
GLdouble colors[] = {
  1,0,0,1,
  1,0,0,1,
  1,0,0,1,
  1,0,0,1,
};
GLdouble vertecies[] = {
  
  0.0, 0.0,
  1.0, 0.0, 
  0.0, 1.0,
  1.0, 1.0, 
};


struct {
  int x,y;
}Player;

int k_any(Ihandle *self, int c, int press)
{
  (c == 0x1000FFE1 || c == 0x1000FFE2)? c -= 0x10000000: c;
  if(c == K_q)
    return IUP_CLOSE;
  
  if((c >= K_A && c <= K_Z) || (c >= K_a && c <= K_z) ){
    switch(c){
      case K_D:{
          Player.x += 34;
        break;
      }
    }
  }
  printf("%x %d  %d\n", c,c, press);
  return IUP_CONTINUE;
}

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

mat4 Mmat = {};
mat4 Pmat = {};
int height, width;
GLuint Shader_program;
int map(Ihandle *self)
{
  int err;
  IupGLMakeCurrent(self);
  
  glExt_cast( glGenBuffers, PFNGLGENBUFFERSPROC);
  glExt_cast( glBindBuffer ,PFNGLBINDBUFFERPROC);
  glExt_cast( glDeleteBuffers, PFNGLDELETEBUFFERSPROC);
  glExt_cast( glBufferData, PFNGLBUFFERDATAPROC);
  glExt_cast( glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
  glExt_cast( glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);
  glExt_cast( glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
  glExt_cast( glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC);
  glExt_cast( glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
  glExt_cast( glBufferSubData, PFNGLBUFFERSUBDATAPROC);
  glExt_cast( glActiveTexture, PFNGLACTIVETEXTUREPROC);
  glExt_cast( glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC);
  glExt_cast( glCreateProgram, PFNGLCREATEPROGRAMPROC);
  glExt_cast( glGetProgramiv, PFNGLGETPROGRAMIVPROC);
  glExt_cast( glShaderSource, PFNGLSHADERSOURCEPROC);
  glExt_cast( glCreateShader, PFNGLCREATESHADERPROC);
  glExt_cast( glCompileShader, PFNGLCOMPILESHADERPROC);
  glExt_cast( glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
  glExt_cast( glAttachShader, PFNGLATTACHSHADERPROC);
  glExt_cast( glLinkProgram, PFNGLLINKPROGRAMPROC);
  glExt_cast( glUseProgram, PFNGLUSEPROGRAMPROC);
  glExt_cast( glGetAttribLocation, PFNGLGETATTRIBLOCATIONPROC);
  glExt_cast( glBindAttribLocation, PFNGLBINDATTRIBLOCATIONPROC);
  glExt_cast( glGenerateMipmap, PFNGLGENERATEMIPMAPPROC); 
  glExt_cast( glUniformMatrix4fv,  PFNGLUNIFORMMATRIX4FVPROC);
  glExt_cast( glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);

  
  imImage *im = imFileImageLoad("Wheat.png", 0, &err);
  if(im == NULL) printf("No Wheat!");
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);printf("HEEEEEY!!!\n");
  int glformat;
  GLubyte* gldata = imImageGetOpenGLData(im, &glformat);
  glTexImage2D(GL_TEXTURE_2D, 0, glformat, im->width, im->height, 0, glformat, GL_UNSIGNED_BYTE, gldata);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glGenerateMipmap(GL_TEXTURE_2D);
  height = im->height, width = im->width; printf("%d %d\n" , height, width);
  
  imImageDestroy(im);

  const char 
  *vertex_shader = 
  "#version 330 core\n"
  "layout (location = 0) in vec2 pos;\n"
  "layout (location = 1) in vec2 txc;\n"
  "layout (location = 2) in vec4 clr;\n"
  "uniform mat4 mod;\n"
  "uniform mat4 pro;\n"
  "out vec4 f_clr;\n"
  "out vec2 f_txc;\n"
  "void main()\n"
  "{\n"
  "   f_clr = clr;\n"
  "   f_txc = txc;\n"
  "   gl_Position = pro*mod*vec4(pos, 0, 1.0);  \n"
  "}", 

 *fragment_shader =
  "#version 330 core\n"
  "in vec4 f_clr;\n"
  "in vec2 f_txc;\n"
  "out vec4 color;\n"
  "uniform sampler2D tex;\n"
  "void main()\n"
  "{\n"
  "  color = f_clr*texture2D(tex, f_txc);\n"
  "}";
  mat4_identity(Mmat);
  mat4_identity(Pmat);
  mat4_Ortho(Pmat, 0, 800, 600, 0, 1, 0);

  Shader_program = glCreateProgram();
  GLuint v_shade = glCreateShader(GL_VERTEX_SHADER),
         f_shade = glCreateShader(GL_FRAGMENT_SHADER);
  
  glShaderSource(v_shade, 1, &vertex_shader, NULL);
  glCompileShader(v_shade);

  glShaderSource(f_shade, 1, &fragment_shader, NULL);
  glCompileShader(f_shade);
  
  glAttachShader(Shader_program, v_shade);
  glAttachShader(Shader_program, f_shade);

  glLinkProgram(Shader_program);
  glUseProgram(Shader_program);
  int pos = glGetAttribLocation(Shader_program, "pos");
  int texc = glGetAttribLocation(Shader_program, "txc");
  int clr = glGetAttribLocation(Shader_program, "clr");
  printf("pos %d, texc %d, clr %d\n", pos, texc, clr);
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertecies) + sizeof(texcoords) + sizeof(colors), 0, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertecies), vertecies);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertecies), sizeof(texcoords), texcoords);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertecies) + sizeof(texcoords), sizeof(colors), colors);
  
  
  glVertexAttribPointer(pos, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), 0);
  glVertexAttribPointer(texc, 2, GL_DOUBLE, GL_TRUE, 2*sizeof(GLdouble), (void*)(sizeof(vertecies)));
  glVertexAttribPointer(clr, 4, GL_DOUBLE, GL_TRUE, 4*sizeof(GLdouble), (void*)(sizeof(vertecies) + sizeof(texcoords)));
  
  
  glEnableVertexAttribArray ( pos );
  glEnableVertexAttribArray ( texc );
  glEnableVertexAttribArray ( clr );
  
  
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  
  return IUP_DEFAULT;
}


int update(Ihandle *self)
{
  
  
  
  return IUP_DEFAULT;
}

int redraw(Ihandle *self, float x, float y)
{
  IupGLMakeCurrent(self);
  
  
  glClearColor(0.0, 0.0, 0.0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mat4 pop = {};
  mat4_clone(pop, Mmat);
  glBindTexture(GL_TEXTURE_2D, tex);
  vec2 n = {32,32};
  mat4_Translate2D(pop,pop, n);
  mat4_Scale2D(pop, pop, n);
  glUseProgram(Shader_program);
  glUniformMatrix4fv( glGetUniformLocation(Shader_program, "pro"), 1, 0, Pmat);
  glUniformMatrix4fv( glGetUniformLocation(Shader_program, "mod"), 1, 0, pop);
  glBindVertexArray(vao);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  IupGLSwapBuffers(self); 

    return IUP_DEFAULT;
}


int resize(Ihandle* self, int width, int height)
{

  glViewport(0, 0, 800, 600);
  
  
  mat4_identity(Mmat);
  mat4_identity(Pmat);
  mat4_Ortho(Pmat, 0, 800, 600, 0, 1, 0);
  return 0;
}


int main(int argc, char **argv)
{
  Ihandle *canvas, *dg, *upd;

  
  
  
  IupOpen(&argc, &argv);
  IupGLCanvasOpen();

  canvas = IupGLCanvas(NULL);
  IupSetCallback(canvas, "ACTION", (Icallback) redraw);
  IupSetAttribute(canvas, IUP_BUFFER, IUP_DOUBLE);
  IupSetAttribute(canvas, "RASTERSIZE", "800x600");
  IupSetCallback(canvas, "K_ANY", (Icallback)k_any);
  IupSetCallback(canvas, "MAP_CB", (Icallback)map);
  IupSetCallback(canvas, "RESIZE_CB", (Icallback)resize);

  

  dg = IupDialog(canvas);
  IupSetAttribute(dg, "TITLE", "IupGLCanvas");

  upd = IupTimer();
  IupSetAttribute(upd, "TIME", "10");
  IupSetAttribute(upd, "RUN", "YES");
  IupSetCallback(upd, "ACTION_CB", (Icallback) update);

  
  
  IupShow(dg);
  IupSetFocus(canvas);
  
  IupMainLoop();
  IupClose();

  glDeleteBuffers(1, &vbo);
  
  
  return EXIT_SUCCESS;
}