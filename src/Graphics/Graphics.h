// Graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#define GLEW_STATIC

#include "src/luadef.h"
#include "src/Graphics/mat4.h"
#include "src/Graphics/glew.h"
#include <windows.h>



extern const GLdouble gvertex[24], tverts[12];

extern struct Entity{
    GLuint vao, vbo, sh;
    struct {
        GLuint pnt, clr, pro, mod;
    }uniforms;
}Dyn, Sta, Prim;

extern GLdouble colors[24], texcoords[12], text_uv[12]; // arrays that change over time
extern mat4 Pmat;
extern mat4 Mmat;

typedef struct _Drawable{
    GLuint tex;
    size_t width, height;
    lua_CFunction method;
    lua_Number **src;
}Drawable;



typedef struct _Vertex{
    GLdouble pos[2], uv[4], color[4];
}Vertex;

#include "src/Graphics/Text.h"
#include "src/Graphics/Sprite.h"
#include "src/Graphics/Batch.h"


int luaopen_Graphics(lua_L);

int Graphics_checkargs(lua_L, const char* func, int num, const char* str);

#endif