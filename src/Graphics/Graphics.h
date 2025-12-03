// Graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H



#include "src/luadef.h"
#include "src/Graphics/mat4.h"
#ifdef __EMSCRIPTEN__
    #include <emscripten/html5_webgl.h>

    #include <webgl/webgl2.h>

#else
    #define GLEW_STATIC
    #include "src/Graphics/glew.h"
    #include <GL/gl.h>
#endif



#ifdef _WIN32
#include <windows.h>
#endif

extern const GLfloat gvertex[24], tverts[12];

extern struct Entity{
    GLuint vao, vbo, sh;
    struct {
        GLint pnt, clr, pro, mod;
    }uniforms;
    struct {
        GLint pos, txc;
    }attr;
}Dyn, Sta, Prim, Bat, Text;

extern GLfloat colors[24], texcoords[12], text_uv[12]; // arrays that change over time
extern mat4 Pmat;
extern mat4 Mmat;

typedef struct _Drawable{
    GLuint tex;
    int width, height;
    lua_CFunction method;
    float **src;
}Drawable;



typedef struct _Vertex{
    GLfloat pos[2], uv[4], color[4];
}Vertex;

#include "src/Graphics/Text.h"
#include "src/Graphics/Sprite.h"
#include "src/Graphics/Batch.h"


int luaopen_Graphics(lua_L);

int Graphics_checkargs(lua_L, const char* func, int num, const char* str);

#endif